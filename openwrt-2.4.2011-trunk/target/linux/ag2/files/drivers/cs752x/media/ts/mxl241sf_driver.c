/*-
 * Copyright (C) 2011 Encore Interactive Inc.
 * All rights reserved.
 *
 * Jared D. McNeill <jared.mcneill@encoreinteractive.ca>
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include "dvb_frontend.h"

#include "MxL241SF_PhyCtrlApi.h"
#include "mxl241sf.h"

#define	MXL241SF_QAM_LOCK_TIMEO	150
#define	MXL241SF_FEC_LOCK_TIMEO	50

static int debug = 0;
static int bandwidth_mhz = 6;

#define	dprintk(args...)						\
	do {								\
		if (debug) printk(KERN_DEBUG "mxl241sf: " args);	\
	} while (0)

struct mxl241sf_softc {
	struct dvb_frontend		sc_frontend;
	struct i2c_adapter		*sc_i2c;
	const struct mxl241sf_config	*sc_config;

	uint8_t				sc_bandwidth_mhz;
	uint32_t			sc_current_freq;
};

#define	MXL241SF_INIT_I2C(_sc, _i2c)					\
	do {								\
		(_i2c)->SlaveAddr = (_sc)->sc_config->demod_address;	\
		(_i2c)->Priv = (_sc)->sc_i2c;				\
	} while (0)
#define	MXL241SF_IS_ANNEX_A(_sc)					\
	((_sc)->sc_bandwidth_mhz == 8)

static int
mxl241sf_xtal_freq(uint32_t freq)
{
	switch (freq) {
	case 16000000:	return XTAL_16MHz;
	case 20000000:	return XTAL_20MHz;
	case 20250000:	return XTAL_20_25MHz;
	case 20480000:	return XTAL_20_48MHz;
	case 24000000:	return XTAL_24MHz;
	case 25000000:	return XTAL_25MHz;
	case 25140000:	return XTAL_25_14MHz;
	case 27000000:	return XTAL_27MHz;
	case 28800000:	return XTAL_28_8MHz;
	case 32000000:	return XTAL_32MHz;
	case 40000000:	return XTAL_40MHz;
	case 44000000:	return XTAL_44MHz;
	case 48000000:	return XTAL_48MHz;
	case 49381100:	return XTAL_49_3811MHz;
	default:	return -1;
	}
}

static bool
mxl241sf_check_qam_lock(struct mxl241sf_softc *sc)
{
	MXL_STATUS status;
	MXL_DEMOD_LOCK_STATUS_T demod_lock_status;
	int retry = MXL241SF_QAM_LOCK_TIMEO;

	MXL241SF_INIT_I2C(sc, &demod_lock_status.I2c);
	while (retry-- > 0) {
		status = MxLWare_API_GetDemodStatus(MXL_DEMOD_QAM_LOCK_REQ,
		    &demod_lock_status);
		if (status == MXL_TRUE &&
		    demod_lock_status.Status == MXL_LOCKED) {
			return true;
		}
		udelay(1000);
	}
	return false;
}

static bool
mxl241sf_check_fec_lock(struct mxl241sf_softc *sc)
{
	MXL_STATUS status;
	MXL_DEMOD_LOCK_STATUS_T fec_lock_status;
	int retry = MXL241SF_FEC_LOCK_TIMEO;

	MXL241SF_INIT_I2C(sc, &fec_lock_status.I2c);
	while (retry-- > 0) {
		status = MxLWare_API_GetDemodStatus(MXL_DEMOD_FEC_LOCK_REQ,
		    &fec_lock_status);
		if (status == MXL_TRUE &&
		    fec_lock_status.Status == MXL_LOCKED) {
			return true;
		}
		udelay(1000);
	}
	return false;
}

static void
mxl241sf_release(struct dvb_frontend *fe)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	kfree(sc);
}

static int
mxl241sf_init(struct dvb_frontend *fe)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	MXL_STATUS status;
	MXL_RESET_CFG_T reset_cfg;
	MXL_OVERWRITE_DEFAULT_CFG_T overwrite_default_cfg;
	MXL_XTAL_CFG_T xtal_cfg;
	MXL_AGC_T agc;
	MXL_TOP_MASTER_CFG_T top_master_cfg;
	MXL_MPEG_OUT_CFG_T mpeg_out_cfg;

	/* SW Reset */
	MXL241SF_INIT_I2C(sc, &reset_cfg.I2c);
	status = MxLWare_API_ConfigDevice(MXL_DEV_SOFT_RESET_CFG, &reset_cfg);
	if (status != MXL_TRUE) {
		printk("%s: MXL_DEV_SOFT_RESET_CFG failed\n", __func__);
		return -ENODEV;
	}

	/* Overwrite default */
	MXL241SF_INIT_I2C(sc, &overwrite_default_cfg.I2c);
	status = MxLWare_API_ConfigDevice(MXL_DEV_OVERWRITE_DEFAULT_CFG,
	    &overwrite_default_cfg);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEV_OVERWRITE_DEFAULT_CFG failed\n", __func__);
		return -ENODEV;
	}

	/* XTAL and clock out setting */
	MXL241SF_INIT_I2C(sc, &xtal_cfg.I2c);
	xtal_cfg.XtalEnable = MXL_ENABLE;
	xtal_cfg.DigXtalFreq = mxl241sf_xtal_freq(sc->sc_config->xtal_freq);
	xtal_cfg.XtalBiasCurrent = 1;
	xtal_cfg.XtalCap = 13;
	xtal_cfg.XtalClkOutEnable = MXL_ENABLE;
	xtal_cfg.XtalClkOutGain = 0xa;
	xtal_cfg.LoopThruEnable = MXL_DISABLE;
	status = MxLWare_API_ConfigDevice(MXL_DEV_XTAL_SETTINGS_CFG, &xtal_cfg);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEV_XTAL_SETTINGS_CFG failed\n", __func__);
		return -ENODEV;
	}

	/* AGC configuration */
	MXL241SF_INIT_I2C(sc, &agc.I2c);
	agc.FreezeAgcGainWord = MXL_NO_FREEZE;
	status = MxLWare_API_ConfigTuner(MXL_TUNER_AGC_SETTINGS_CFG, &agc);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_TUNER_AGC_SETTINGS_CFG failed\n", __func__);
		return -ENODEV;
	}

	/* Power up tuner */
	MXL241SF_INIT_I2C(sc, &top_master_cfg.I2c);
	top_master_cfg.TopMasterEnable = MXL_ENABLE;
	status = MxLWare_API_ConfigTuner(MXL_TUNER_TOP_MASTER_CFG,
	    &top_master_cfg);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_TUNER_TOP_MASTER_CFG failed\n", __func__);
		return -ENODEV;
	}

	/* MPEG out setting */
	MXL241SF_INIT_I2C(sc, &mpeg_out_cfg.I2c);
	mpeg_out_cfg.SerialOrPar = MPEG_DATA_SERIAL;//MPEG_DATA_PARALLEL;
	mpeg_out_cfg.LsbOrMsbFirst = MPEG_SERIAL_MSB_1ST;
	mpeg_out_cfg.MpegSyncPulseWidth = MPEG_SYNC_WIDTH_BIT;
	mpeg_out_cfg.MpegValidPol = MPEG_ACTIVE_HIGH;
	mpeg_out_cfg.MpegClkPol = MPEG_CLK_POSITIVE;
	mpeg_out_cfg.MpegSyncPol = MPEG_ACTIVE_HIGH;
	mpeg_out_cfg.MpegClkFreq = MPEG_CLK_57MHz;
	mpeg_out_cfg.MpegClkSource = MPEG_CLK_INTERNAL;
	status = MxLWare_API_ConfigDemod(MXL_DEMOD_MPEG_OUT_CFG, &mpeg_out_cfg);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_MPEG_OUT_CFG failed\n", __func__);
		return -ENODEV;
	}

	return 0;
}

static int
mxl241sf_sleep(struct dvb_frontend *fe)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	MXL_STATUS status;
	MXL_PWR_MODE_CFG_T pwr_mode_cfg;

	MXL241SF_INIT_I2C(sc, &pwr_mode_cfg.I2c);
	pwr_mode_cfg.PowerMode = STANDBY_ON;
	status = MxLWare_API_ConfigDevice(MXL_DEV_POWER_MODE_CFG,
	    &pwr_mode_cfg);

	return status == MXL_TRUE ? 0 : -EIO;
}

static int
mxl241sf_set_frontend(struct dvb_frontend *fe, struct dvb_frontend_parameters *params)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	MXL_STATUS status;
	MXL_SYMBOL_RATE_T symbol_rate;
	MXL_ANNEX_CFG_T annex_cfg;
	MXL_RF_TUNE_CFG_T rf_tune_cfg;
	MXL_ADCIQFLIP_CFG_T adciqflip_cfg;
	MXL_CHAN_DEPENDENT_T chan_dependent;
	MXL_INVERT_CARR_OFFSET_T invert_carr_offset;

	/* Config symbol rate */
	MXL241SF_INIT_I2C(sc, &symbol_rate.I2c);
	if (MXL241SF_IS_ANNEX_A(sc))
		symbol_rate.SymbolType = SYM_TYPE_J83A;
	else
		symbol_rate.SymbolType = SYM_TYPE_J83B;
	status = MxLWare_API_ConfigDemod(MXL_DEMOD_SYMBOL_RATE_CFG,
	    &symbol_rate);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_SYMBOL_RATE_CFG failed\n", __func__);
		return -EIO;
	}

	/* Config annex type */
	MXL241SF_INIT_I2C(sc, &annex_cfg.I2c);
	if (MXL241SF_IS_ANNEX_A(sc))
		annex_cfg.AnnexType = ANNEX_A;
	else
		annex_cfg.AnnexType = ANNEX_B;
	annex_cfg.AutoDetectQamType = MXL_ENABLE;
	annex_cfg.AutoDetectMode = MXL_ENABLE;
	status = MxLWare_API_ConfigDemod(MXL_DEMOD_ANNEX_QAM_TYPE_CFG,
	    &annex_cfg);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_ANNEX_QAM_TYPE_CFG failed\n", __func__);
		return -EIO;
	}

	/* Tune RF with channel frequency and bandwidth */
	MXL241SF_INIT_I2C(sc, &rf_tune_cfg.I2c);
	rf_tune_cfg.BandWidth = sc->sc_bandwidth_mhz;
	rf_tune_cfg.Frequency = params->frequency;
	status = MxLWare_API_ConfigTuner(MXL_TUNER_CHAN_TUNE_CFG, &rf_tune_cfg);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_TUNER_CHAN_TUNE_CFG failed\n", __func__);
		return -EIO;
	}

	msleep(25);

	/* Configure spectum inversion */
	MXL241SF_INIT_I2C(sc, &adciqflip_cfg.I2c);
	adciqflip_cfg.AdcIqFlip =
	    MXL241SF_IS_ANNEX_A(sc) ? MXL_ENABLE : MXL_DISABLE;
	status = MxLWare_API_ConfigDemod(MXL_DEMOD_ADC_IQ_FLIP_CFG,
	    &adciqflip_cfg);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_ADC_IQ_FLIP_CFG failed\n", __func__);
		return -EIO;
	}

	/* Channel dependent setting */
	MXL241SF_INIT_I2C(sc, &chan_dependent.I2c);
	chan_dependent.ChanDependentCfg = MXL_DISABLE;
	status = MxLWare_API_ConfigTuner(MXL_TUNER_CHAN_DEPENDENT_TUNE_CFG,
	    &chan_dependent);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_TUNER_CHAN_DEPENDENT_TUNE_CFG failed\n",
		    __func__);
		return -EIO;
	}

	/* Auto spectrum inversion */
	if (mxl241sf_check_qam_lock(sc) == true &&
	    mxl241sf_check_fec_lock(sc) == false) {
		/* Invert carrier offset */
		MXL241SF_INIT_I2C(sc, &invert_carr_offset.I2c);
		status = MxLWare_API_ConfigDemod(
		    MXL_DEMOD_INVERT_CARRIER_OFFSET_CFG, &invert_carr_offset);
		if (status != MXL_TRUE) {
			dprintk("%s: MXL_DEMOD_INVERT_CARRIER_OFFSET_CFG failed\n",
			    __func__);
			return -EIO;
		}

		/* toggle I/Q path flip */
		adciqflip_cfg.AdcIqFlip =
		    (adciqflip_cfg.AdcIqFlip == MXL_ENABLE) ? MXL_DISABLE :
		                                              MXL_ENABLE;
		status = MxLWare_API_ConfigDemod(
		    MXL_DEMOD_ADC_IQ_FLIP_CFG, &adciqflip_cfg);
		if (status != MXL_TRUE) {
			dprintk("%s: MXL_DEMOD_ADC_IQ_FLIP_CFG failed\n",
			    __func__);
			return -EIO;
		}
	}

	sc->sc_current_freq = params->frequency;

	return 0;
}

static int
mxl241sf_get_frontend(struct dvb_frontend *fe, struct dvb_frontend_parameters *params)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	MXL_STATUS status;
	MXL_DEMOD_ANNEXQAM_INFO_T demod_annexqam_info;

	params->frequency = sc->sc_current_freq;

	/* Get current QAM info */
	MXL241SF_INIT_I2C(sc, &demod_annexqam_info.I2c);
	status = MxLWare_API_ConfigDemod(MXL_DEMOD_ANNEX_QAM_TYPE_CFG,
	    &demod_annexqam_info);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_ANNEX_QAM_TYPE_CFG failed\n", __func__);
		return -EIO;
	}

	switch (demod_annexqam_info.QamType) {
	case MXL_QAM16:		params->u.qam.modulation = QAM_16;	break;
	case MXL_QAM64:		params->u.qam.modulation = QAM_64;	break;
	case MXL_QAM256:	params->u.qam.modulation = QAM_256;	break;
#ifdef notyet
	case MXL_QAM1024:	params->u.qam.modulation = QAM_1024;	break;
#endif
	case MXL_QAM32:		params->u.qam.modulation = QAM_32;	break;
	case MXL_QAM128:	params->u.qam.modulation = QAM_128;	break;
	case MXL_QPSK:		params->u.qam.modulation = QPSK;	break;
	default:
		dprintk("%s: unsupported MXL_QAM_TYPE_E (%d)\n",
		    __func__, demod_annexqam_info.QamType);
		return -EINVAL;
	}

	return 0;
}

static int
mxl241sf_get_tune_settings(struct dvb_frontend *fe, struct dvb_frontend_tune_settings *fesettings)
{
	fesettings->min_delay_ms = 100;	/* XXXJDM */
	return 0;
}

static int
mxl241sf_read_status(struct dvb_frontend *fe, fe_status_t *festatus)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	MXL_STATUS status;
	MXL_DEMOD_LOCK_STATUS_T demod_lock_status, fec_lock_status,
				mpeg_lock_status;

	MXL241SF_INIT_I2C(sc, &demod_lock_status.I2c);
	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_QAM_LOCK_REQ,
	    &demod_lock_status);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_QAM_LOCK_REQ failed\n", __func__);
		return -EIO;
	}

	MXL241SF_INIT_I2C(sc, &fec_lock_status.I2c);
	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_FEC_LOCK_REQ,
	    &fec_lock_status);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_FEC_LOCK_REQ failed\n", __func__);
		return -EIO;
	}

	MXL241SF_INIT_I2C(sc, &mpeg_lock_status.I2c);
	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_MPEG_LOCK_REQ,
	    &mpeg_lock_status);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_MPEG_LOCK_REQ failed\n", __func__);
		return -EIO;
	}

	/* XXX */
	if (demod_lock_status.Status == MXL_LOCKED)
		*festatus |= (FE_HAS_SIGNAL | FE_HAS_CARRIER);
	if (fec_lock_status.Status == MXL_LOCKED)
		*festatus |= (FE_HAS_LOCK | FE_HAS_VITERBI);
	if (mpeg_lock_status.Status == MXL_LOCKED)
		*festatus |= FE_HAS_SYNC;

	return 0;
}

static int
mxl241sf_read_ber(struct dvb_frontend *fe, uint32_t *ber)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	MXL_STATUS status;
	MXL_DEMOD_BER_INFO_T demod_ber_info;

	MXL241SF_INIT_I2C(sc, &demod_ber_info.I2c);
	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_BER_UNCODED_BER_CER_REQ,
	    &demod_ber_info);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_BER_UNCODED_BER_CER_REQ failed\n",
		    __func__);
		return -EIO;
	}
	*ber = (uint32_t)demod_ber_info.BER;

	return 0;
}

static int
mxl241sf_read_snr(struct dvb_frontend *fe, uint16_t *snr)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	MXL_STATUS status;
	MXL_DEMOD_SNR_INFO_T demod_snr_info;

	MXL241SF_INIT_I2C(sc, &demod_snr_info.I2c);
	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_SNR_REQ, &demod_snr_info);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_SNR_REQ failed\n", __func__);
		return -EIO;
	}

	*snr = demod_snr_info.SNR;

	return 0;
}

static int
mxl241sf_read_signal_strength(struct dvb_frontend *fe, uint16_t *strength)
{
	return mxl241sf_read_snr(fe, strength);
}

static int
mxl241sf_read_ucblocks(struct dvb_frontend *fe, uint32_t *ucblocks)
{
	struct mxl241sf_softc *sc = fe->tuner_priv;
	MXL_STATUS status;
	MXL_DEMOD_BER_INFO_T demod_ber_info;

	MXL241SF_INIT_I2C(sc, &demod_ber_info.I2c);
	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_BER_UNCODED_BER_CER_REQ,
	    &demod_ber_info);
	if (status != MXL_TRUE) {
		dprintk("%s: MXL_DEMOD_BER_UNCODED_BER_CER_REQ failed\n",
		    __func__);
		return -EIO;
	}
	*ucblocks = (uint32_t)demod_ber_info.UncodedBER;

	return 0;
}

static const struct dvb_frontend_ops mxl241sf_ops;

struct dvb_frontend *
mxl241sf_attach(const struct mxl241sf_config *config, struct i2c_adapter *i2c)
{
	struct mxl241sf_softc *sc = NULL;
	MXL_STATUS status;
	MXL_DEV_INFO_T dev_info;

	sc = kzalloc(sizeof(*sc), GFP_KERNEL);
	if (sc == NULL)
		goto failed;

	sc->sc_config = config;
	sc->sc_i2c = i2c;
	sc->sc_frontend.ops = mxl241sf_ops;
	sc->sc_frontend.tuner_priv = sc;

	/* Host bridge must supply a sane xtal freq value */
	if (mxl241sf_xtal_freq(sc->sc_config->xtal_freq) == -1) {
		printk(KERN_ERR "%s: unsupported xtal freq %u\n",
		    __func__, sc->sc_config->xtal_freq);
		goto failed;
	}
	/* Bandwidth (MHz) must be 6 (US Cable) or 8 (DVB-C) */
	if (bandwidth_mhz != 6 && bandwidth_mhz != 8) {
		printk(KERN_ERR
		    "%s: unsupported 'bandwidth_mhz' %d (must be 6 or 8)\n",
		    __func__, bandwidth_mhz);
		goto failed;
	}
	sc->sc_bandwidth_mhz = bandwidth_mhz;

	if (mxl241sf_init(&sc->sc_frontend) != 0) {
		printk(KERN_ERR "%s: couldn't initialize device\n", __func__);
		goto failed;
	}

	/* Get device ID and version */
	MXL241SF_INIT_I2C(sc, &dev_info.I2c);
	status = MxLWare_API_GetDeviceStatus(MXL_DEV_ID_VERSION_REQ, &dev_info);
	if (status == MXL_TRUE) {
		printk(KERN_INFO "%s: DevId = 0x%x, Version = 0x%x\n",
		    __func__, dev_info.DevId, dev_info.DevVer);
	}

	return &sc->sc_frontend;

failed:
	kfree(sc);
	dprintk("%s: couldn't attach frontend\n", __func__);
	return NULL;
}

static const struct dvb_frontend_ops mxl241sf_ops = {
	.info = {
		.name = "MaxLinear MxL241SF Frontend",
		.type = FE_QAM,
		.frequency_min =   44000000,
		.frequency_max = 1002000000,
		.frequency_stepsize = 0,
		.symbol_rate_min =  722000,	/* OOB */
		.symbol_rate_max = 6890000,	/* AnnexA */
		.caps = FE_CAN_QAM_64 | FE_CAN_QAM_256 | FE_CAN_QAM_AUTO |
			FE_CAN_INVERSION_AUTO,
	},

	.release = mxl241sf_release,
	.init = mxl241sf_init,
	.sleep = mxl241sf_sleep,

	.set_frontend = mxl241sf_set_frontend,
	.get_frontend = mxl241sf_get_frontend,
	.get_tune_settings = mxl241sf_get_tune_settings,
	.read_status = mxl241sf_read_status,
	.read_ber = mxl241sf_read_ber,
	.read_signal_strength = mxl241sf_read_signal_strength,
	.read_snr = mxl241sf_read_snr,
	.read_ucblocks = mxl241sf_read_ucblocks,
};

module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Toggle mxl241sf driver debugging (default: off)");
module_param(bandwidth_mhz, int, 0644);
MODULE_PARM_DESC(bandwidth_mhz, "Set QAM bandwidth in MHz (default: 6)");

MODULE_DESCRIPTION("MaxLinear MxL241SF Tuner and Demodulator Driver");
MODULE_AUTHOR("Jared D. McNeill <jared.mcneill@encoreinteractive.ca>");
MODULE_LICENSE("Copyright (C) 2011 Encore Interactive Inc.");

EXPORT_SYMBOL(mxl241sf_attach);
