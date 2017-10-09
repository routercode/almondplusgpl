/*******************************************************************************
 *
 * FILE NAME          : MxL241SF_PhyCtrlApi.cpp
 * 
 * AUTHOR             : Brenndon Lee
 * DATE CREATED       : 6/22/2009
 *
 * DESCRIPTION        : This file contains MxL241SF driver APIs
 *                             
 *******************************************************************************
 *                Copyright (c) 2006, MaxLinear, Inc.
 ******************************************************************************/

#include "MxL241SF_PhyCtrlApi.h"
#include "MxL241SF_PhyCfg.h"
#include "MxL241SF_OEM_Drv.h"

/* MxLWare Driver version for MxL241SF */
const UINT8 MxLWareDrvVersion[] = {2, 1, 15, 1};

/* For statistics */
ACC_STAT_COUNTER_T AccStatCounter[MAX_241SF_DEVICES];

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDevReset - MXL_DEV_SOFT_RESET_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : By writing any value into address 0xFFFF(AIC), all control 
--|                 registers are initialized to the default value.
--|                 AIC - Address Initiated Command
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigDevReset(PMXL_RESET_CFG_T ResetParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 i;

  /* Stop Tune */
  status |= Ctrl_WriteRegister(ResetParamPtr->I2c, RF_SEQUENCER_REG, STOP_TUNE);
printk("%s: status 1 = %x......\n",__func__,status);
  
  /* Power down Tuner */
  status |= Ctrl_WriteRegister(ResetParamPtr->I2c, TOP_MASTER_CONTROL_REG, DISABLE_TOP_MASTER);
printk("%s: status 2 = %x......\n",__func__,status);

  status |= Ctrl_WriteRegister(ResetParamPtr->I2c, AIC_RESET_REG, 0x0000);
printk("%s: status 3 = %x......\n",__func__,status);
  
  /* Statistic variables are also initialized */
  for (i = 0; i < MAX_241SF_DEVICES; i++)
  {
    if (AccStatCounter[i].I2c.SlaveAddr == ResetParamPtr->I2c.SlaveAddr) break;
  }

  if (i == MAX_241SF_DEVICES)
  {
    /* This Device was never registered */
    /* Find empty slot */
    for (i = 0; i < MAX_241SF_DEVICES; i++)
    {
      if (AccStatCounter[i].I2c.SlaveAddr == 0) break;
    }

    /* No empty slot */
    if (i == MAX_241SF_DEVICES)
    {
printk("*** %s : return MXL_FALSE ***\n",__func__);
      return MXL_FALSE;
    }
  }

  /* Register this I2C address */
  AccStatCounter[i].I2c = ResetParamPtr->I2c;

  AccStatCounter[i].AccCwCorrCount = 0;
  AccStatCounter[i].AccCwErrCount = 0;
  AccStatCounter[i].AccCwUnerrCount = 0;
  AccStatCounter[i].AccCwReceived = 0;
  AccStatCounter[i].AccCorrBits = 0;
  AccStatCounter[i].AccErrMpeg = 0;
  AccStatCounter[i].AccReceivedMpeg = 0;

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDeviceInfo - MXL_DEV_ID_VERSION_REQ
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : This function returns MxL241SF Chip Id and version information.
--|                 Device Id of MxL241SF is 0x1, Version is unknown yet 
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetDeviceInfo(PMXL_DEV_INFO_T DevInfoPtr)
{
  MXL_STATUS status;
  UINT16 readBack;
  UINT8 i = 0;

  status = Ctrl_ReadRegister(DevInfoPtr->I2c, VERSION_REG, &readBack);

  DevInfoPtr->DevId = (UINT8)(readBack & 0xF);
  DevInfoPtr->DevVer = (UINT8)((readBack >> 4) & 0xF);

  /* MxLWare Driver Version */
  for (i = 0 ; i < sizeof(MxLWareDrvVersion) ; i++)
  {
    DevInfoPtr->MxLWareVer[i] = MxLWareDrvVersion[i];
  }

  MxL_DLL_DEBUG0("Device Id = 0x%d, Device Version = 0x%x", DevInfoPtr->DevId, DevInfoPtr->DevVer);    
  MxL_DLL_DEBUG0("MxLWare Drv Ver = v%d,%d,%d,%d\n", DevInfoPtr->MxLWareVer[0], \
                                                     DevInfoPtr->MxLWareVer[1], \
                                                     DevInfoPtr->MxLWareVer[2], \
                                                     DevInfoPtr->MxLWareVer[3]);
  
  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDevXtalSettings - MXL_DEV_XTAL_SETTINGS_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : This function configures XTAL frequency, Clok-out settings, and
--|                 Loop through control for MxL241SF.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigDevXtalSettings(PMXL_XTAL_CFG_T XtalParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 control;

  MxL_DLL_DEBUG0("MXL_DEV_XTAL_SETTINGS_CFG - XtalEnable = %d, XtalFreq=%d, XtalCap=%d, XtalBias=%d, \
                                              XtalClkOut=%d, XtalClkOutGain=%d, LoopThrough=%d",
                                              XtalParamPtr->XtalEnable, XtalParamPtr->DigXtalFreq, XtalParamPtr->XtalCap, XtalParamPtr->XtalBiasCurrent,
                                              XtalParamPtr->XtalClkOutEnable, XtalParamPtr->XtalClkOutGain, XtalParamPtr->LoopThruEnable);

  /* XTAL Enable or Disable */
  if (XtalParamPtr->XtalEnable == MXL_ENABLE) control = ENABLE_XTAL;
  else control = 0;

  /* Set XTAL Frequency */
  control |= XtalParamPtr->DigXtalFreq;

  status |= Ctrl_WriteRegister(XtalParamPtr->I2c, DIG_XTAL_FREQ_CTRL_REG, control);

  /* Xtal Capacitor */
  control = (UINT16)XtalParamPtr->XtalCap;
  status |= Ctrl_WriteRegister(XtalParamPtr->I2c, DIG_XTAL_CAP_CTRL_REG, control);

  /* Xtal Bias */
  control = (UINT16)XtalParamPtr->XtalBiasCurrent << 4;
  status |= Ctrl_WriteRegister(XtalParamPtr->I2c, DIG_XTAL_BIAS_CTRL_REG, control);

  /* Xtal CLK Out */
  if (XtalParamPtr->XtalClkOutEnable == MXL_ENABLE) control = ENABLE_CLK_OUT;
  else control = 0;

  control |= (UINT16)XtalParamPtr->XtalClkOutGain;
  status |= Ctrl_WriteRegister(XtalParamPtr->I2c, XTAL_CLK_OUT_CTRL_REG, control);

  /* Loop Through */
  control = 0x14;

  if (XtalParamPtr->LoopThruEnable == MXL_ENABLE) control |= ENABLE_LOOP_THRU;

  status |= Ctrl_WriteRegister(XtalParamPtr->I2c, LOOP_THROUGH_CTRL_REG, control);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDemodQamBurstFreeze - MXL_DEMOD_QAM_BURST_FREEZE_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 5/25/2010
--|
--| DESCRIPTION   : It sets demod reaction speed against QAM BURST noise.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS MxL_ConfigDemodQamBurstFreeze(PMXL_QAM_BURST_FREEZE_T ParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 readData;

  status |= Ctrl_ReadRegister(ParamPtr->I2c, QAM_BURST_FREEZE, &readData);

  /* Reset */
  readData &= 0x9FFF;

  /* QAM Burst Freeze enable <13> */
  if (ParamPtr->QamFreezeEnable == MXL_ENABLE)
  {
    readData |= 0x2000;
  }

  /* QAM Burst Freeze mode <14> */
  if (ParamPtr->QamFreezeMode == MXL_ENABLE)
  {
    readData |= 0x4000;
  }

  status |= Ctrl_WriteRegister(ParamPtr->I2c, QAM_BURST_FREEZE, readData);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDemodInvertCarrierOffset - MXL_DEMOD_INVERT_CARRIER_OFFSET_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 4/13/2011
--|
--| DESCRIPTION   : It inverts demod carrier offset.
--|                 When auto spectrum algorithm used, calling this API will able to 
--|                 reduece locking time
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS MxL_ConfigDemodInvertCarrierOffset(PMXL_INVERT_CARR_OFFSET_T ParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 regData;
  
  /* Before readback, register 0x803E needs to be written by data 0x0087  */
  status |= Ctrl_WriteRegister(ParamPtr->I2c, CARRIER_OFFSET_REG, 0x0087);

  /* Read the current settings. */
  status |= Ctrl_ReadRegister(ParamPtr->I2c, CARRIER_OFFSET_RB_REG, &regData);

  status |= Ctrl_WriteRegister(ParamPtr->I2c, CARRIER_OFFSET_RB_REG, (SINT16)regData * (-1));
  
  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDemodSymbolRate - MXL_DEMOD_SYMBOL_RATE_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/19/2009
--|                 6/02/2010
--|
--| DESCRIPTION   : This function configures Symbol Rate for Annex A/B or OOB, and
--|                 supports either float or integer calculation depending on
--|                 conditional compile (__MXL_INTEGER_CALC_STATISTICS__)
--|
--|                 Resample register bank - 0 : Annex-A bank
--|                                          1 : Annex B 64 QAM
--|                                          2 : Annex B 256 QAM
--|                 Interpolator bypass - 0 : bypass off
--|                                       1 : bypass on
--|                 Symbol rate - 0 : 6.89 MHz (A)
--|                               1 : 5.05 MHz (B64)
--|                               2 : 5.36 Mhz (B256)
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

#ifdef __MXL_INTEGER_CALC_STATISTICS__
MXL_STATUS MxL_ConfigDemodSymbolRate(PMXL_SYMBOL_RATE_T SymbolRatePtr) 
{
  UINT8 status = MXL_TRUE;
  PREG_CTRL_INFO_T OobFilterCoefDataPtr = NULL;
  UINT32 SymbolRate[SYM_NUM_B_BANK];
  UINT32 ResampleRateRatio;
  UINT16 ResampleRegCtrl, control, sweepStep = 0;
  UINT8 i, NumBank, Bank[SYM_NUM_B_BANK];
  MXL_QAM_BURST_FREEZE_T qamBurstFreeze;
  
  MxL_DLL_DEBUG0("MXL_DEMOD_SYMBOL_RATE_CFG - SymbolType=%d, SymbolRate=0x%x, SymbolRate256=0x%x, OobSymbolRate=%d\n",
     SymbolRatePtr->SymbolType, SymbolRatePtr->SymbolRate, SymbolRatePtr->SymbolRate256, SymbolRatePtr->OobSymbolRate);

  switch (SymbolRatePtr->SymbolType)
  {
    case SYM_TYPE_J83A:
      SymbolRate[0] = 0x0108BB43;
      Bank[0] = 0; /* Annex-A bank */
      NumBank = 1;
      break;

    case SYM_TYPE_J83B:
      NumBank = 2;
      SymbolRate[0] = 0x0168B13F;
      Bank[0] = 1; /* Annex-B 64QAM bank */

      SymbolRate[1] = 0x015443B1;
      Bank[1] = 2; /* Annex-B 256QAM bank */
      break;

    case SYM_TYPE_OOB:
      NumBank = 1;
      Bank[0] = 0; /* Annex-A bank */
      switch (SymbolRatePtr->OobSymbolRate)
      {
        case SYM_RATE_0_772MHz:
          SymbolRate[0] = 0x03139094;
          OobFilterCoefDataPtr = MxL_OobAciMfCoef_0_772MHz;
          sweepStep = 0x0009;
          break;

        case SYM_RATE_1_024MHz:
          SymbolRate[0] = 0x0251C000;
          OobFilterCoefDataPtr = MxL_OobAciMfCoef_1_024MHz;
          sweepStep = 0x000A;
          break;

        case SYM_RATE_1_544MHz:
          SymbolRate[0] = 0x0189C84A;
          OobFilterCoefDataPtr = MxL_OobAciMfCoef_1_544MHz;
          sweepStep = 0x0009;
          break;

        default:
          return MXL_FALSE;
      }
      break;

    default:
      return MXL_FALSE;
       break;
  }

  /* Configure Resample rate ratio  */
  for (i = 0; i < NumBank; i++)
  {
    /* Read the current settings. */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, SYMBOL_RATE_RESAMP_BANK_REG, &ResampleRegCtrl);
    ResampleRegCtrl &= 0xFFF8;

    /* Select Bank */
    ResampleRegCtrl |= Bank[i]; 

    /* INTERP_BYPASS <4> */
    if (SymbolRatePtr->SymbolType == SYM_TYPE_OOB)
    {
      ResampleRegCtrl |= 0x0010;
    }
    else
    {
      ResampleRegCtrl &= 0xFFEF;
    }

    /* Configure Resample_register bank and Interpolator bypass control  */
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, SYMBOL_RATE_RESAMP_BANK_REG, ResampleRegCtrl);

    ResampleRateRatio = SymbolRate[i];
    MxL_DLL_DEBUG0("ResampleRateRatio = 0x%x\n", ResampleRateRatio);

    /* Configure Resample Rate Ratio register */
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, 
                                SYMBOL_RATE_RESAMP_RATE_LO_REG, 
                                ResampleRateRatio & 0xFF);
    ResampleRateRatio >>= 8;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, 
                                SYMBOL_RATE_RESAMP_RATE_MID_LO_REG, 
                                ResampleRateRatio & 0xFF);
    ResampleRateRatio >>= 8;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, 
                                SYMBOL_RATE_RESAMP_RATE_MID_HI_REG, 
                                ResampleRateRatio & 0xFF);
    ResampleRateRatio >>= 8;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, 
                                SYMBOL_RATE_RESAMP_RATE_HI_REG, 
                                ResampleRateRatio & 0xFF);
  }

  if (SYM_TYPE_OOB == SymbolRatePtr->SymbolType)
  {
    /* Set Annex-A and QPSK  */
    control = MXL_QPSK;
    control |= ANNEX_A_TYPE;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, QAM_ANNEX_TYPE_REG, control);

    /* Watch dog disable <12> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, QAM_TYPE_AUTO_DETECT_REG, &control);
    control &= 0xEFFF;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, QAM_TYPE_AUTO_DETECT_REG, control);

    /* Enable Custom ACI COEF <5> */
    /* Enable Matched Filter custom coefficients <6> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, CUSTOM_COEF_EN_REG, &control);
    control |= 0x0060;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, CUSTOM_COEF_EN_REG, control);

    /* Set max = 15 Equalizer frequency sweep limit <7:4> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, EQU_FREQ_SWEEP_LIMIT_REG, &control);
    control |= 0x00F0;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, EQU_FREQ_SWEEP_LIMIT_REG, control);

    /* Sweep step <3:0> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, EQU_FREQ_SWEEP_LIMIT_REG, &control);
    control &= 0xFFF0;
    control |= sweepStep;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, EQU_FREQ_SWEEP_LIMIT_REG, control);

    /* Set serial output interface <1> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, MPEGOUT_PAR_CLK_CTRL_REG, &control);
    control &= 0xFFFD;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, MPEGOUT_PAR_CLK_CTRL_REG, control);

    /* Enable 2X drive for MCLK and MDAT <7:0> */
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, DIG_PIN_2XDRV_REG, 0x0002);

    /* Configure control before writing filter data */
    status |= Ctrl_ProgramRegisters(SymbolRatePtr->I2c, MxL_OobAciMfCoef);

    /* Program multiple registers for the specified function */
    status |= Ctrl_ProgramRegisters(SymbolRatePtr->I2c, OobFilterCoefDataPtr);
  }
  else
  {
    // Wider blind state frequency sweep range for Annex-B mode
    if (SYM_TYPE_J83B == SymbolRatePtr->SymbolType)
    {
      MxL_EqualizerSpeedUp[0].data = WIDER_FREQ_SWEEP_RANGE;
    }
    else
    {
      MxL_EqualizerSpeedUp[0].data = DEFAULT_FREQ_SWEEP_RANGE;
    }

    status |= Ctrl_ProgramRegisters(SymbolRatePtr->I2c, MxL_EqualizerSpeedUp);

    /* QAM Burst Freeze cfg only for J.83 A/B */
    qamBurstFreeze.I2c = SymbolRatePtr->I2c;
    qamBurstFreeze.QamFreezeEnable = MXL_ENABLE;
    qamBurstFreeze.QamFreezeMode = MXL_ENABLE;
    status |= MxL_ConfigDemodQamBurstFreeze(&qamBurstFreeze);
  }

  return (MXL_STATUS)status;
}
#else
MXL_STATUS MxL_ConfigDemodSymbolRate(PMXL_SYMBOL_RATE_T SymbolRatePtr) 
{
  UINT8 status = MXL_TRUE;
  PREG_CTRL_INFO_T OobFilterCoefDataPtr = MxL_OobAciMfCoef_0_772MHz;
  REAL32 SPSin, SymbolRate[SYM_NUM_B_BANK];
  UINT32 ResampleRateRatio;
  UINT16 ResampleRegCtrl, control, sweepStep;
  UINT8 i, NumBank, Bank[SYM_NUM_B_BANK];
  MXL_QAM_BURST_FREEZE_T qamBurstFreeze;

  MxL_DLL_DEBUG0("MXL_DEMOD_SYMBOL_RATE_CFG - SymbolType=%d, SymbolRate=%f, SymbolRate256=%f, OobSymbolRate=%d",
     SymbolRatePtr->SymbolType, SymbolRatePtr->SymbolRate, SymbolRatePtr->SymbolRate256, SymbolRatePtr->OobSymbolRate);

  switch (SymbolRatePtr->SymbolType)
  {
    case SYM_TYPE_J83A:
      SymbolRate[0] = (REAL32)6.89;
      Bank[0] = 0; /* Annex-A bank */
      NumBank = 1;
      break;

    case SYM_TYPE_J83B:
      NumBank = 2;
      SymbolRate[0] = (REAL32)5.056941;
      Bank[0] = 1; /* Annex-B 64QAM bank */

      SymbolRate[1] = (REAL32)5.360537;
      Bank[1] = 2; /* Annex-B 256QAM bank */
      break;

    case SYM_TYPE_USER_DEFINED_J83A:
      NumBank = 1;
      SymbolRate[0] = SymbolRatePtr->SymbolRate;
      Bank[0] = 0; /* Annex-A bank */
      break;

    case SYM_TYPE_USER_DEFINED_J83B:
      NumBank = 2;
      SymbolRate[0] = SymbolRatePtr->SymbolRate;
      Bank[0] = 1; /* Annex-B 64QAM bank */
      SymbolRate[1] = SymbolRatePtr->SymbolRate256;
      Bank[1] = 2; /* Annex-B 256QAM bank */
      break;

    case SYM_TYPE_OOB:
      NumBank = 1;
      Bank[0] = 0; /* Annex-A bank */
      switch (SymbolRatePtr->OobSymbolRate)
      {
        case SYM_RATE_0_772MHz:
          SymbolRate[0] = (REAL32)0.772;
          OobFilterCoefDataPtr = MxL_OobAciMfCoef_0_772MHz;
          sweepStep = 0x0009;
          break;

        case SYM_RATE_1_024MHz:
          SymbolRate[0] = (REAL32)1.024;
          OobFilterCoefDataPtr = MxL_OobAciMfCoef_1_024MHz;
          sweepStep = 0x000A;
          break;

        case SYM_RATE_1_544MHz:
          SymbolRate[0] = (REAL32)1.544;
          OobFilterCoefDataPtr = MxL_OobAciMfCoef_1_544MHz;
          sweepStep = 0x0009;
          break;

        default:
          return MXL_FALSE;
      }
      break;

    default:
      return MXL_FALSE;
       break;
  }

  /* Configure Resample rate ratio  */
  for (i = 0; i < NumBank; i++)
  {
    /* Read the current settings. */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, SYMBOL_RATE_RESAMP_BANK_REG, &ResampleRegCtrl);
    ResampleRegCtrl &= 0xFFF8;

    /* Select Bank */
    ResampleRegCtrl |= Bank[i]; 

    if (0 != SymbolRate[i]) SPSin = (REAL32)9.5 / SymbolRate[i];
    else status |= MXL_FALSE;

    ResampleRateRatio = 1 << 24;
    SymbolRate[i] = (REAL32)ResampleRateRatio * SPSin;

    if (SPSin <= 4)
    {
      /* Interpolator X3 should not be bypassed  */
      ResampleRegCtrl &= ~BYPASS_INTERPOLATOR;
      SymbolRate[i] *= 0.75;
    }
    else
    {
      /* Interpolator X3 should be bypassed  */
      ResampleRegCtrl |= BYPASS_INTERPOLATOR;
      SymbolRate[i] /= 4;
    }

    /* Configure Resample_register bank and Interpolator bypass control  */
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, SYMBOL_RATE_RESAMP_BANK_REG, ResampleRegCtrl);

    ResampleRateRatio = (UINT32)SymbolRate[i];
    MxL_DLL_DEBUG0("SPSin = %f, ResampleRateRatio = 0x%x", SPSin, ResampleRateRatio);

    /* Configure Resample Rate Ratio register */
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, 
                                SYMBOL_RATE_RESAMP_RATE_LO_REG, 
                                ResampleRateRatio & 0xFF);
    ResampleRateRatio >>= 8;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, 
                                SYMBOL_RATE_RESAMP_RATE_MID_LO_REG, 
                                ResampleRateRatio & 0xFF);
    ResampleRateRatio >>= 8;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, 
                                SYMBOL_RATE_RESAMP_RATE_MID_HI_REG, 
                                ResampleRateRatio & 0xFF);
    ResampleRateRatio >>= 8;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, 
                                SYMBOL_RATE_RESAMP_RATE_HI_REG, 
                                ResampleRateRatio & 0xFF);
  }

  if (SYM_TYPE_OOB == SymbolRatePtr->SymbolType)
  {
    /* Set Annex-A and QPSK  */
    control = MXL_QPSK;
    control |= ANNEX_A_TYPE;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, QAM_ANNEX_TYPE_REG, control);

    /* Watch dog disable <12> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, QAM_TYPE_AUTO_DETECT_REG, &control);
    control &= 0xEFFF;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, QAM_TYPE_AUTO_DETECT_REG, control);

    /* Enable Custom ACI COEF <5> */
    /* Enable Matched Filter custom coefficients <6> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, CUSTOM_COEF_EN_REG, &control);
    control |= 0x0060;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, CUSTOM_COEF_EN_REG, control);

    /* Set max = 15 Equalizer frequency sweep limit <7:4> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, EQU_FREQ_SWEEP_LIMIT_REG, &control);
    control |= 0x00F0;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, EQU_FREQ_SWEEP_LIMIT_REG, control);

    /* Sweep step <3:0> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, EQU_FREQ_SWEEP_LIMIT_REG, &control);
    control &= 0xFFF0;
    control |= sweepStep;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, EQU_FREQ_SWEEP_LIMIT_REG, control);

    /* Set serial output interface <1> */
    status |= Ctrl_ReadRegister(SymbolRatePtr->I2c, MPEGOUT_PAR_CLK_CTRL_REG, &control);
    control &= 0xFFFD;
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, MPEGOUT_PAR_CLK_CTRL_REG, control);

    /* Enable 2X drive for MCLK and MDAT <7:0> */
    status |= Ctrl_WriteRegister(SymbolRatePtr->I2c, DIG_PIN_2XDRV_REG, 0x0002);

    /* Configure control before writing filter data */
    status |= Ctrl_ProgramRegisters(SymbolRatePtr->I2c, MxL_OobAciMfCoef);

    /* Program multiple registers for the specified function */
    status |= Ctrl_ProgramRegisters(SymbolRatePtr->I2c, OobFilterCoefDataPtr);
  }
  else
  {
    // Wider blind state frequency sweep range for Annex-B mode
    if (SYM_TYPE_J83B == SymbolRatePtr->SymbolType)
    {
      MxL_EqualizerSpeedUp[0].data = WIDER_FREQ_SWEEP_RANGE;
    }
    else
    {
      MxL_EqualizerSpeedUp[0].data = DEFAULT_FREQ_SWEEP_RANGE;
    }

    status = Ctrl_ProgramRegisters(SymbolRatePtr->I2c, MxL_EqualizerSpeedUp);

    /* QAM Burst Freeze cfg only for J.83 A/B */
    qamBurstFreeze.I2c = SymbolRatePtr->I2c;
    qamBurstFreeze.QamFreezeEnable = MXL_ENABLE;
    qamBurstFreeze.QamFreezeMode = MXL_ENABLE;
    status |= MxL_ConfigDemodQamBurstFreeze(&qamBurstFreeze);
  }

  return (MXL_STATUS)status;
}
#endif

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigTunerAgc - MXL_TUNER_AGC_SETTINGS_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : This function configures AGC set-point and AGC mode
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigTunerAgc(PMXL_AGC_T AgcParamPtr) 
{
  MXL_STATUS status;
  UINT16 control;

  MxL_DLL_DEBUG0("MXL_TUNER_AGC_SETTINGS_CFG - FreezeGainWord=%d", \
                     AgcParamPtr->FreezeAgcGainWord);

  /* AGC Gain word control*/
  /* 35 < AGC setpoint < 45 */
  if (AgcParamPtr->FreezeAgcGainWord == MXL_FREEZE) control = FREEZE_AGC_GAIN_WORD;
  else control = 0;
                                               
  status = Ctrl_WriteRegister(AgcParamPtr->I2c, DIG_HALT_GAIN_CTRL_REG, control);

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigTunerTopMaster - MXL_TUNER_TOP_MASTER_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/22/2009
--|
--| DESCRIPTION   : Enabling or Disabling Tuner Block
--|                 
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigTunerTopMaster(PMXL_TOP_MASTER_CFG_T TopMasterCfgPtr) 
{
  MXL_STATUS status;
  UINT16 control;

  if (TopMasterCfgPtr->TopMasterEnable == MXL_ENABLE) control = ENABLE_TOP_MASTER;
  else control = DISABLE_TOP_MASTER;
  
  status = Ctrl_WriteRegister(TopMasterCfgPtr->I2c, TOP_MASTER_CONTROL_REG, control);

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDemodAnnexQamType - MXL_DEMOD_ANNEX_QAM_TYPE_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : This function configure QAM_ANNEX_TYPE and QAM type
--|                 Annex-A type coressponds to J.83 A/C QAM, and OOB signal
--|                 Annex-B for J.83 B QAM signal
--|                 QAM Type : 0 - 16 QAM, 1 - 64 QAM, 2 - 256 QAM,
--|                            3 - 1024 QAM, 4 - 32 QAM, 5 - 128 QAM,
--|                            6 - QPSK
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigDemodAnnexQamType(PMXL_ANNEX_CFG_T AnnexQamTypePtr) 
{
  UINT8 status = MXL_TRUE;
  UINT16 AnnnexType, AutoDetectCtrl;

  MxL_DLL_DEBUG0("MXL_DEMOD_ANNEX_QAM_TYPE_CFG - AnnexType=%d, AutoDetectQamType=%d, AutoDetectMode=%d, QamType=%d",
                  AnnexQamTypePtr->AnnexType, AnnexQamTypePtr->AutoDetectQamType, AnnexQamTypePtr->AutoDetectMode, AnnexQamTypePtr->QamType);    

  /* Read the current settings. */
  status |= Ctrl_ReadRegister(AnnexQamTypePtr->I2c, QAM_ANNEX_TYPE_REG, &AnnnexType);

  if (AnnexQamTypePtr->AnnexType == ANNEX_A) AnnnexType |= ANNEX_A_TYPE;
  else AnnnexType &= ~ANNEX_A_TYPE; /* Annex-B  */

  status = Ctrl_ReadRegister(AnnexQamTypePtr->I2c, QAM_TYPE_AUTO_DETECT_REG, &AutoDetectCtrl);
  /* AutoDetectCtrl = 0x0014; */

  if (AnnexQamTypePtr->AutoDetectQamType == MXL_ENABLE)
  {
    AutoDetectCtrl |= ENABLE_AUTO_DETECT_QAM_TYPE; /* Enable Auto detect QAM Type */

    if (AnnexQamTypePtr->AutoDetectMode == MXL_ENABLE) AutoDetectCtrl |= ENABLE_AUTO_DETECT_MODE; /* Enable Auto-detect mode  */
    else AutoDetectCtrl &= ~ENABLE_AUTO_DETECT_MODE; /* Disbale Auto-detect mode  */
  }
  else
  { 
    AutoDetectCtrl &= ~ENABLE_AUTO_DETECT_QAM_TYPE; /* Disable Auto detect QAM Type */

    /* Set QAM type manually  */
    /* Reset Qam type <2:0>   */
    AnnnexType &= 0xFFF8;
    AnnnexType |= AnnexQamTypePtr->QamType;
  }
  
  /* Configure Annex Type  */
  status |= Ctrl_WriteRegister(AnnexQamTypePtr->I2c, QAM_ANNEX_TYPE_REG, AnnnexType);

  /* Configure Auto detect mode  */
  status |= Ctrl_WriteRegister(AnnexQamTypePtr->I2c, QAM_TYPE_AUTO_DETECT_REG, AutoDetectCtrl);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDemodMpegOutIface - MXL_DEMOD_MPEG_OUT_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : For Transport stream output through MPEG TS interface
--|                 the following parameters are needed to configure after or before
--|                 demod lock.
--|                  TS output mode : Seral or Parallel
--|                  MSB or LSB first      : In serial output mode
--|                  MPEG Valid Polarity   : Active Low or High
--|                  MPEG Clock Polarity   : Rising or Falling Edge
--|                  MPEG Sync Polarity    : Active Low or High
--|                  MPEG Sync Pulse width : 1 bit or 8 bit, only for serial mode
--|                  MPEG CLK Frequency  : 0 - 57MHz (6x9.5)(default)
--|               (Internally generated) : 1 - 38MHz (4x9.5)
--|                                      : 2 - 28.5MHz (3x9.5)
--|                                      : 3 - 19MHz (2x9.5)
--|                                      : 4 - 9.5MHz
--|                                      : 5 - 7.125MHz (3/4x9.5)
--|                                      : 6 - 4.75MHz  (1/2x9.5)
--|                                      : 7 - 3.5625MHz (3/8x9.5)
--|                  MPEG Clock Source   : External or Internal MPEG Clock
--|                  External MPEG Clock Phase   : In phase or Inverted phase of the externally provided clock
--|
--| RETURN VALUE  : true or false
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigDemodMpegOutIface(PMXL_MPEG_OUT_CFG_T MpegOutParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 control;
  MXL_I2C_CONTROLLER i2c = MpegOutParamPtr->I2c;

  MxL_DLL_DEBUG0("MXL_DEMOD_MPEG_OUT_CFG - SerialOrPar=%d, LsbOrMsbFirst=%d, MpegValidPol=%d, MpegClkPol=%d, MpegSyncPol=%d\n MpegSyncPulseWidth=%d, MpegClkSource = %d, MpegClkFreq=%d, MpegErrorIndication=%d",
                 MpegOutParamPtr->SerialOrPar,
                 MpegOutParamPtr->LsbOrMsbFirst,
                 MpegOutParamPtr->MpegValidPol,
                 MpegOutParamPtr->MpegClkPol,
                 MpegOutParamPtr->MpegSyncPol,
                 MpegOutParamPtr->MpegSyncPulseWidth,
                 MpegOutParamPtr->MpegClkSource,
                 MpegOutParamPtr->MpegClkFreq,
                 MpegOutParamPtr->MpegErrorIndication);

  /* Clock source  */
  if (MpegOutParamPtr->MpegClkSource == MPEG_CLK_INTERNAL) control = MPEG_CLK_INTERNAL;
  else control = MPEG_CLK_EXTERNAL; /* Use external MPEG Clock */

  /* MPEG Clock Frequency */
  control |= (MpegOutParamPtr->MpegClkFreq << 2);

  if (MpegOutParamPtr->SerialOrPar == MPEG_DATA_PARALLEL)
  {
    /* Parallel */
    control |= (MPEG_DATA_PARALLEL << 1);

    /* MPEG Clock Frequency */
    control |= (MpegOutParamPtr->MpegClkFreq << 2);

    /* MPEG Clock polarity */
    if (MpegOutParamPtr->MpegClkPol == MPEG_CLK_NEGATIVE) control |= (MPEG_CLK_NEGATIVE << 5);

    /* MPEG Data Valid level */
    if (MpegOutParamPtr->MpegValidPol == MPEG_ACTIVE_HIGH) control |= (MPEG_ACTIVE_HIGH << 6);

    /* MPEG Sync Level */
    if (MpegOutParamPtr->MpegSyncPol == MPEG_ACTIVE_HIGH) control |= (MPEG_ACTIVE_HIGH << 7);
  }

  status |= Ctrl_WriteRegister(i2c, MPEGOUT_PAR_CLK_CTRL_REG, control);

  if (MpegOutParamPtr->MpegErrorIndication == MXL_ENABLE) control = ENABLE_MPEG_ERROR_IND;
  else control = 0;

  /* If MPEG Data out format is serial, configure for phase and polarity */
  if (MpegOutParamPtr->SerialOrPar == MPEG_DATA_SERIAL)
  {
    /* MPEG Data Valid level */
    if (MpegOutParamPtr->MpegValidPol == MPEG_ACTIVE_LOW) control |= MPEG_ACTIVE_HIGH;

    /* MPEG Sync Level */
    if (MpegOutParamPtr->MpegSyncPol == MPEG_ACTIVE_LOW) control |= (MPEG_ACTIVE_HIGH << 1);

    /* LSB or MSB first */
    if (MpegOutParamPtr->LsbOrMsbFirst == MPEG_SERIAL_MSB_1ST) control |= (MPEG_SERIAL_MSB_1ST << 2);

    /* SYNC Pulse width */
    if (MpegOutParamPtr->MpegSyncPulseWidth == MPEG_SYNC_WIDTH_BYTE) control |= (MPEG_SYNC_WIDTH_BYTE << 3);

   /* MPEG Clock polarity */
    if (MpegOutParamPtr->MpegClkPol == MPEG_CLK_NEGATIVE) control |= (MPEG_CLK_NEGATIVE << 4);
  }
  else
  {
    /* For parallel, DISABLE_FIFO_READ_LIMIT has to be set */
    control |= FIFO_READ_UNLIMIT;
  }

  status |= Ctrl_WriteRegister(i2c, MPEGOUT_SER_CTRL_REG, control);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDevPowerSavingMode - MXL_DEV_POWER_MODE_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/19/2009
--|                 2/14/2009
--|                 3/02/2010
--|
--| DESCRIPTION   : This function configures Standby mode and Sleep mode to
--|                 control power consumption.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigDevPowerSavingMode(PMXL_PWR_MODE_CFG_T PwrModePtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 control;

  switch (PwrModePtr->PowerMode)
  {
    case STANDBY_ON:
    case SLEEP_ON:
      /* Sequencer settings  */
      status |= Ctrl_WriteRegister(PwrModePtr->I2c, RF_SEQUENCER_REG, STOP_TUNE); 
      
      /* Power down Tuner  */
      status |= Ctrl_WriteRegister(PwrModePtr->I2c, TOP_MASTER_CONTROL_REG, DISABLE_TOP_MASTER);

      /* REG_BIAS = 0 <5:4> */
      status |= Ctrl_ReadRegister(PwrModePtr->I2c, DN_LNA_BIAS_CTRL_REG, &control);
      control &= 0x00CF;
      status |= Ctrl_WriteRegister(PwrModePtr->I2c, DN_LNA_BIAS_CTRL_REG, control);

      /* Enable Loop Through  */
      status |= Ctrl_ReadRegister(PwrModePtr->I2c, LOOP_THROUGH_CTRL_REG, &control);
      control &= ~ENABLE_LOOP_THRU;
      control |= ENABLE_LOOP_THRU;
      status |= Ctrl_WriteRegister(PwrModePtr->I2c, LOOP_THROUGH_CTRL_REG, control);

      if (PwrModePtr->PowerMode == SLEEP_ON)
      {
        /* Disable Clock Out  */
        status |= Ctrl_ReadRegister(PwrModePtr->I2c, XTAL_CLK_OUT_CTRL_REG, &control);
        control &= ~ENABLE_CLK_OUT;

        status |= Ctrl_WriteRegister(PwrModePtr->I2c, XTAL_CLK_OUT_CTRL_REG, control);
      }
      break;

    case STANDBY_OFF:
    case SLEEP_OFF:
      /* REG_BIAS = 2 <5:4> */
      status |= Ctrl_ReadRegister(PwrModePtr->I2c, DN_LNA_BIAS_CTRL_REG, &control);
      control &= 0x00CF;
      control |= 0x0020;
      status |= Ctrl_WriteRegister(PwrModePtr->I2c, DN_LNA_BIAS_CTRL_REG, control);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDevOverwriteDefault - MXL_DEV_OVERWRITE_DEFAULT_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 08/27/2009
--|
--| DESCRIPTION   : Overwrite default value
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigDevOverwriteDefault(PMXL_OVERWRITE_DEFAULT_CFG_T ParamPtr)
{
  MXL_STATUS status;

  status = Ctrl_ProgramRegisters(ParamPtr->I2c, MxL_OverwriteDefaults);

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDemodAdcIqFlip - MXL_DEMOD_ADC_IQ_FLIP_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 06/18/2009
--|
--| DESCRIPTION   : Set ADC IQ Flip enable/disable
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigDemodAdcIqFlip(PMXL_ADCIQFLIP_CFG_T AdcIqFlipPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 control;

  /* Enable = 1, disable = 0
   * Flip the I/Q path after ADC
   * DIG_ADCIQ_FLIP <4>
   */
  status |= Ctrl_ReadRegister(AdcIqFlipPtr->I2c, DIG_ADCIQ_FLIP_REG, &control);

  if (AdcIqFlipPtr->AdcIqFlip == MXL_ENABLE) control |= 0x10;
  else control &= 0xFFEF;

  status |= Ctrl_WriteRegister(AdcIqFlipPtr->I2c, DIG_ADCIQ_FLIP_REG, control);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigTunerChanDependent - MXL_TUNER_CHAN_DEPENDENT_TUNE_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 02/01/2010
--|
--| DESCRIPTION   : This function configures channel dependent tuner setting
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS MxL_ConfigTunerChanDependent(PMXL_CHAN_DEPENDENT_T ParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 tmpData, control;

  /* XTAL_EN_VCO_BIASBST <3> */
  status |= Ctrl_ReadRegister(ParamPtr->I2c, XTAL_EN_VCO_BIASBST, &tmpData);
  tmpData &= 0xFFF7;

  if (ParamPtr->ChanDependentCfg == MXL_ENABLE)
  {
    /* DIG_TEMP_RB <7:4> */
    status |= Ctrl_ReadRegister(ParamPtr->I2c, DIG_TEMP_RB, &control);
    control = (control & 0x00F0) >> 4;

    /* Channel dependent tuner setting */
    if (control < TEMP_THRESHOLD)
    {
      tmpData |= 0x0008;
    }
    status |= Ctrl_WriteRegister(ParamPtr->I2c, XTAL_EN_VCO_BIASBST, tmpData);

    // Increase RF Power within certain range of freq
    if ((44000000 <= ParamPtr->RfFreqHz && 49000000 >= ParamPtr->RfFreqHz) ||      \
        (56000000 <= ParamPtr->RfFreqHz && 66000000 >= ParamPtr->RfFreqHz) ||      \
        (83000000 <= ParamPtr->RfFreqHz && 99000000 >= ParamPtr->RfFreqHz) ||      \
        (111000000 <= ParamPtr->RfFreqHz && 133000000 >= ParamPtr->RfFreqHz) ||    \
        (167000000 <= ParamPtr->RfFreqHz && 199000000 >= ParamPtr->RfFreqHz) ||    \
        (222000000 <= ParamPtr->RfFreqHz && 266000000 >= ParamPtr->RfFreqHz) ||    \
        (333000000 <= ParamPtr->RfFreqHz && 399000000 >= ParamPtr->RfFreqHz) ||    \
        (444000000 <= ParamPtr->RfFreqHz && 533000000 >= ParamPtr->RfFreqHz) ||    \
        (667000000 <= ParamPtr->RfFreqHz && 799000000 >= ParamPtr->RfFreqHz))
    {
      status |= Ctrl_WriteRegister(ParamPtr->I2c, RF_SPARE_REG, 0x00F4);
    }
    else
    {
      status |= Ctrl_WriteRegister(ParamPtr->I2c, RF_SPARE_REG, 0);
    }
  }
  else
  {
    status |= Ctrl_WriteRegister(ParamPtr->I2c, XTAL_EN_VCO_BIASBST, tmpData);
  }

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigTunerChanTune - MXL_TUNER_CHAN_TUNE_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/24/2009
--|                 6/02/2010
--|
--| DESCRIPTION   : This API configures RF channel frequency and bandwidth. 
--|                 Radio Frequency unit is Hz and Bandwidth unit is MHz, and
--|                 supports either float or integer calculation depending on
--|                 conditional compile (__MXL_INTEGER_CALC_STATISTICS__)
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

#ifdef __MXL_INTEGER_CALC_STATISTICS__
MXL_STATUS MxL_ConfigTunerChanTune(PMXL_RF_TUNE_CFG_T TuneParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT64 frequency;
  UINT32 freq = 0; 
  UINT16 bandwidth = 0;
  MXL_ADCIQFLIP_CFG_T AdcIqFlipCfg;
  MXL_CHAN_DEPENDENT_T ChanDependentCfg;

  /* Set ADC IQ Flip to 0  */
  AdcIqFlipCfg.I2c = TuneParamPtr->I2c;
  AdcIqFlipCfg.AdcIqFlip = MXL_DISABLE;

  MxL_DLL_DEBUG0("MXL_TUNER_CHAN_TUNE_CFG - Freq = %dHz, BW = %dMHz\n", TuneParamPtr->Frequency, TuneParamPtr->BandWidth);    

  /* Stop Tune */
  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_SEQUENCER_REG, STOP_TUNE);

  /* Set AGC IQ Flip */
  status |= MxL_ConfigDemodAdcIqFlip(&AdcIqFlipCfg);

  if (TuneParamPtr->BandWidth == 6) bandwidth = 0x49;
  else if (TuneParamPtr->BandWidth == 8) bandwidth = 0x6F;
  else status |= MXL_FALSE; /* BandWidth is not defined */

  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_CHAN_BW_REG, bandwidth);
  
  frequency = TuneParamPtr->Frequency;

  /* Calculate RF Channel = DIV(64*RF(Hz), 1E6) */
  frequency *= 64;

  /* Quotient */
  while (frequency >= 1000000)
  {
    frequency -= 1000000;
    freq++;
  }
 
  /* Set RF  */
  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_LOW_REG, (UINT16)(freq & 0x00FF)); /* Fractional part */
  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_HIGH_REG, (UINT16)((freq >> 8 ) & 0x00FF)); /* Integer part */

  /* Channel dependent tune setting */
  ChanDependentCfg.I2c = TuneParamPtr->I2c;
  ChanDependentCfg.ChanDependentCfg = MXL_ENABLE;
  ChanDependentCfg.RfFreqHz = TuneParamPtr->Frequency;
  status |= MxL_ConfigTunerChanDependent(&ChanDependentCfg);

  /* Start Tune */
  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_SEQUENCER_REG, START_TUNE); 

  return (MXL_STATUS)status;
}
#else
MXL_STATUS MxL_ConfigTunerChanTune(PMXL_RF_TUNE_CFG_T TuneParamPtr)
{
  UINT8 status = MXL_TRUE;
  REAL32 frequency;
  UINT32 freq; 
  UINT16 bandwidth = 0;
  MXL_ADCIQFLIP_CFG_T AdcIqFlipCfg;
  MXL_CHAN_DEPENDENT_T ChanDependentCfg;

  /* Set ADC IQ Flip to 0  */
  AdcIqFlipCfg.I2c = TuneParamPtr->I2c;
  AdcIqFlipCfg.AdcIqFlip = MXL_DISABLE;

  MxL_DLL_DEBUG0("MXL_TUNER_CHAN_TUNE_CFG - Freq = %fHz, BW = %dMHz", TuneParamPtr->Frequency, TuneParamPtr->BandWidth);    

  /* Stop Tune */
  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_SEQUENCER_REG, STOP_TUNE); 

  /* Set AGC IQ Flip */
  status |= MxL_ConfigDemodAdcIqFlip(&AdcIqFlipCfg);

  if (TuneParamPtr->BandWidth == 6) bandwidth = 0x49;
  else if (TuneParamPtr->BandWidth == 8) bandwidth = 0x6F;
  else status |= MXL_FALSE; /* BandWidth is not defined */

  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_CHAN_BW_REG, bandwidth);
  
  frequency = TuneParamPtr->Frequency;

  /* Calculate RF Channel */
  frequency /= 1000000;
  frequency *= 64;

  /* Do round */
  frequency += 0.5;
  freq = (UINT32)frequency;
 
  /* Set RF  */
  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_LOW_REG, (UINT16)(freq & 0x00FF)); /* Fractional part */
  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_HIGH_REG, (UINT16)((freq >> 8 ) & 0x00FF)); /* Integer part */

  /* Channel dependent tune setting */
  ChanDependentCfg.I2c = TuneParamPtr->I2c;
  ChanDependentCfg.ChanDependentCfg = MXL_ENABLE;
  ChanDependentCfg.RfFreqHz = (UINT32)TuneParamPtr->Frequency;
  status |= MxL_ConfigTunerChanDependent(&ChanDependentCfg);

  /* Start Tune */
  status |= Ctrl_WriteRegister(TuneParamPtr->I2c, RF_SEQUENCER_REG, START_TUNE); 

  return (MXL_STATUS)status;
}
#endif

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigTunerAgcLockSpeed - MXL_TUNER_AGC_LOCK_SPEED_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 01/06/2009
--|
--| DESCRIPTION   : This function configures AGC lock speed with
--|                 either fast mode or normal mode
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS MxL_ConfigTunerAgcLockSpeed(PMXL_AGC_CTRL_SPEED_T AgcCtrlSpeedMode)
{
  MXL_STATUS status;

  switch(AgcCtrlSpeedMode->AgcSpeedMode)
  {
    case FAST_AGC_LOCK:
      status = Ctrl_WriteRegister(AgcCtrlSpeedMode->I2c, AGC_CTRL_SPEED_REG, FAST_AGC_CTRL_LOCK);
      break;

    case NORMAL_AGC_LOCK:
      status = Ctrl_WriteRegister(AgcCtrlSpeedMode->I2c, AGC_CTRL_SPEED_REG, NORMAL_AGC_CTRL_LOCK);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ConfigDemodInterrupt - MXL_DEMOD_INTR_MASK_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : This function configures Interrupt Mask register 
--|                 to enable or disable it.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ConfigDemodInterrupt(PMXL_INTR_CFG_T IntrMaskInfoPtr)
{
  MXL_STATUS status;
  UINT16 IntrMask = IntrMaskInfoPtr->IntrMask;

  status = Ctrl_WriteRegister(IntrMaskInfoPtr->I2c, INTR_MASK_REG, IntrMask); 
 
  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodInterruptStatus
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : This function retrieves the interrupt status when Interrupt 
--|                 is triggered by demod
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetDemodInterruptStatus(PMXL_INTR_STATUS_T IntrStatusInfoPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 IntrMask, IntrStatus;

  status |= Ctrl_ReadRegister(IntrStatusInfoPtr->I2c, INTR_STATUS_REG, &IntrStatus);
  status |= Ctrl_ReadRegister(IntrStatusInfoPtr->I2c, INTR_MASK_REG, &IntrMask);

  IntrStatusInfoPtr->IntrStatus = IntrStatus;
  IntrStatusInfoPtr->IntrMask = IntrMask;

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ClearDemodInterrupt - MXL_DEMOD_INTR_CLEAR_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : This function clears the triggered interrupt
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ClearDemodInterrupt(PMXL_INTR_CLEAR_T IntrClearInfoPtr)
{
  MXL_STATUS status;
  UINT16 IntrClear = IntrClearInfoPtr->IntrClear; 

  status = Ctrl_WriteRegister(IntrClearInfoPtr->I2c, INTR_CLEAR_REG, IntrClear); 

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodLockStatus - MXL_DEMOD_QAM_LOCK_REQ
--|                                          MXL_DEMOD_FEC_LOCK_REQ, 
--|                                          MXL_DEMOD_MPEG_LOCK_REQ, 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/24/2009
--|
--| DESCRIPTION   : This function returns QAM, FEC, or MPEG Lock status of Demod.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetDemodLockStatus(MXL_CMD_TYPE_E CmdType, PMXL_DEMOD_LOCK_STATUS_T DemodLockStatusPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 readBack, AnnexType;
  UINT8 lock = 0;
 
  switch (CmdType)
  {
    case MXL_DEMOD_QAM_LOCK_REQ:
      status = Ctrl_ReadRegister(DemodLockStatusPtr->I2c, QAM_LOCK_STATUS_REG, &readBack);
      lock = (UINT8)(readBack & 0x0001);
      break;

    case MXL_DEMOD_MPEG_LOCK_REQ:
      status = Ctrl_ReadRegister(DemodLockStatusPtr->I2c, FEC_MPEG_LOCK_REG, &readBack);
      lock = (UINT8)(readBack & 0x0001);
      break;

    case MXL_DEMOD_FEC_LOCK_REQ:
      /* Check Annex Type and QAM TYPE  */
      status |= Ctrl_ReadRegister(DemodLockStatusPtr->I2c, QAM_ANNEX_TYPE_REG, &readBack); 
      AnnexType = readBack & ANNEX_A_TYPE;    /* Bit 10 of Reg 0x8001 gives ANNEX Type */

      status |= Ctrl_ReadRegister(DemodLockStatusPtr->I2c, FEC_MPEG_LOCK_REG, &readBack);

      lock = (UINT8)((readBack >> 1) & 0x0001);

      if (AnnexType == ANNEX_A_TYPE)
      {
        if (lock == 0) lock = 1; /* Locked to FEC */
        else lock = 0;           /* Unlock */
      }
      break;

    default:
      break;
  }

  DemodLockStatusPtr->Status = (MXL_BOOL)lock;

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodSnr - MXL_DEMOD_SNR_REQ
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/19/2009
--|                 3/02/2010
--|
--| DESCRIPTION   : This function returns SNR(Signal to Noise Ratio), and
--|                 supports either float or integer calculation depending on
--|                 conditional compile (__MXL_INTEGER_CALC_STATISTICS__)
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

#ifdef __MXL_INTEGER_CALC_STATISTICS__
MXL_STATUS MxL_GetDemodSnr(PMXL_DEMOD_SNR_INFO_T SnrPtr)
{
  UINT8 status = MXL_TRUE, N = 16, T = 0, R = 0;
  UINT16 AnnexType, QamType, Y = 0;
  UINT16 RegData, TmpData, control;
  SINT32 L = 0;

  /* Read QAM and Annex type */
  status |= Ctrl_ReadRegister(SnrPtr->I2c, QAM_ANNEX_TYPE_REG, &RegData); 

  AnnexType = (RegData & 0x0400) >> 10;
  QamType = (RegData & 0x0007);

  /* Before reading SNR value, write MSE_AVG_COEF */
  status |= Ctrl_ReadRegister(SnrPtr->I2c, SNR_MSE_AVG_COEF_REG, &control);
  control &= 0x07FF;
  control |= MSE_AVG_COEF;
  status |= Ctrl_WriteRegister(SnrPtr->I2c, SNR_MSE_AVG_COEF_REG, control);

  status |= Ctrl_ReadRegister(SnrPtr->I2c, SNR_EXT_SPACE_ADDR_REG, &RegData);
  RegData &= 0x0100;

  /* Set command to read MSE data from Extended space data register  */
  RegData |= 0x0089;
  status |= Ctrl_WriteRegister(SnrPtr->I2c, SNR_EXT_SPACE_ADDR_REG, RegData);

  status |= Ctrl_ReadRegister(SnrPtr->I2c, SNR_EXT_SPACE_DATA_REG, &RegData);
  
  MxL_DLL_DEBUG0("QAM Type = %d, Annex type = %d, Reg(0x803f) = 0x%x", QamType, AnnexType, RegData);    

  switch (QamType)
  {
    case 0: /* 16 QAM */
      L = -11072;
      break;

    case 1: /* 64 QAM */
      L = -10860;
      break;

    case 2: /* 256 QAM */
      L = -10809;
      break;

    case 4: /* 32 QAM */
      L = -14082;
      break;

    case 5: /* 128 QAM */
      L = -13975;
      break;

    case 6: /* QPSK */
      L = -8519;
      break;

    default:
      status |= MXL_FALSE;
      break;
  }

  /* T = number of bits occupied in X */
  TmpData = RegData;
  while (TmpData != 0)
  {
    TmpData >>= 1;
    T++;
  }

  /* R = T - 5 if T > 5 or R = 0 */
  /* Y = X right shifted by Z-5 bits if T > 5 or X if T <= 5 */
  /* Y becomes the address of MxL_LutYLookUpTable */
  if (T > 5)
  {
    R = T - 5;
    Y = RegData >> (T - 5);
  }
  else
  {
    R = 0;
    Y = RegData;
  }

  /* 1000 * SNR = (-1) * LUT(Y) + 3010 * (N + 4 - R ) + L */
  SnrPtr->SNR = (-1) * MxL_LutYLookUpTable[Y].lutY + 3010 * (N + 4 - R) + L;

  MxL_DLL_DEBUG0("Device = 0x%x, SNR = %ddB\n", SnrPtr->I2c.SlaveAddr, SnrPtr->SNR);    

  return (MXL_STATUS)status;
}
#else
MXL_STATUS MxL_GetDemodSnr(PMXL_DEMOD_SNR_INFO_T SnrPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 AnnexType, QamType, Es, M;
  UINT16 RegData, control;
  REAL32 Mse, constant, k;
  UINT32 CalcRes;

  /* Read QAM and Annex type */
  status |= Ctrl_ReadRegister(SnrPtr->I2c, QAM_ANNEX_TYPE_REG, &RegData); 

  AnnexType = (RegData & 0x0400) >> 10;
  QamType = (RegData & 0x0007);

  /* Before reading SNR value, write MSE_AVG_COEF */
  status |= Ctrl_ReadRegister(SnrPtr->I2c, SNR_MSE_AVG_COEF_REG, &control);
  control &= 0x07FF;
  control |= MSE_AVG_COEF;
  status |= Ctrl_WriteRegister(SnrPtr->I2c, SNR_MSE_AVG_COEF_REG, control);

  status |= Ctrl_ReadRegister(SnrPtr->I2c, SNR_EXT_SPACE_ADDR_REG, &RegData);
  RegData &= 0x0100;

  /* Set command to read MSE data from Extended space data register  */
  RegData |= 0x0089;
  status |= Ctrl_WriteRegister(SnrPtr->I2c, SNR_EXT_SPACE_ADDR_REG, RegData);

  status |= Ctrl_ReadRegister(SnrPtr->I2c, SNR_EXT_SPACE_DATA_REG, &RegData);
  
  MxL_DLL_DEBUG0("QAM Type = %d, Annex type = %d, Reg(0x803f) = 0x%x", QamType, AnnexType, RegData);    

  switch (QamType)
  {
    case 0: /* 16 QAM */
      M = 8;  
      Es = 10;
      break;

    case 1: /* 64 QAM */
      M = 16;  
      Es = 42;
      break;

    case 2: /* 256 QAM */
      M = 32;  
      Es = 170;
      break;

    case 4: /* 32 QAM */
      M = 16;  
      Es = 20;
      break;

    case 5: /* 128 QAM */
      M = 32;  
      Es = 82;
      break;

    case 6: /* QPSK */
      M = 8;  
      Es = 18;
      break;

    default:
      M = 1;  
      Es = 1;
      break;
  }

  /*
   * MSE = 10log10(RegData * 2^ -(N + Log2(1/16)) * 8/K)
   * SNR = -1 * MSE
   * N = 16 (No. of Bits)
   * K = Es/(M/2)2
   * E & M change with QAM types
   */
  constant = 1.0/1048576; /* 2 ^(-N + Log2(1/16); */
  k = (REAL32)pow((M/2), 2.0);
  k = (REAL32)(Es)/k;

  if (RegData != 0)
  {
    Mse = 10 * (REAL32)log10((REAL32)RegData * constant * (8/k));
    SnrPtr->SNR = -1 * Mse;
  }
  else
  {
    SnrPtr->SNR = 0.0;
  }

  /* SNR/MSE size (num of symbols) */
  status |= Ctrl_ReadRegister(SnrPtr->I2c, PHY_EQUALIZER_LLP_CONTROL_1_DEBUG_MSE_REGISTER, &RegData);
  RegData = (RegData & 0xF800) >> 11;
  CalcRes = (RegData + 1)/2 + 5;
  /* pow(2, CalcRes) */
  SnrPtr->MseSize = 1 << CalcRes;
 
  MxL_DLL_DEBUG0("Device = 0x%x, SNR = %fdB\n", SnrPtr->I2c, SnrPtr->SNR);    
 
  return (MXL_STATUS)status;       
}
#endif

/*------------------------------------------------------------------------------
--| FUNCTION NAME : GetMCNSSD
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 1/24/2008
--|
--| DESCRIPTION   : Read back Counter registers for BER, CER calculation.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS GetMCNSSD(MXL_I2C_CONTROLLER I2c, UINT8 RegCode, UINT32 *dataPtr)
{
  UINT8 status = MXL_TRUE;
  UINT32 tmp;
  UINT16 lsb;
  UINT16 msb;

  switch (RegCode)
  {
    case CW_CORR_COUNT:
      /* CW_CORR_COUNT = MSB * 2^16 +  LSB */
      /*  Get LSB */
      status |= Ctrl_WriteRegister(I2c, MCNSSD_SEL_REG, 0x0002);
      status |= Ctrl_ReadRegister(I2c, MCNSSD_REG, &lsb);
      /* Get MSB */
      status |= Ctrl_WriteRegister(I2c, MCNSSD_SEL_REG, 0x0003);
      status |= Ctrl_ReadRegister(I2c, MCNSSD_REG, &msb);
      tmp = msb << 16;
      tmp |= lsb;
      break;

    case CW_ERR_COUNT:
      /* CW_ERR_COUNT = MSB * 2^16 +  LSB */
      /* Get LSB */
      status |= Ctrl_WriteRegister(I2c, MCNSSD_SEL_REG, 0x0004);
      status |= Ctrl_ReadRegister(I2c, MCNSSD_REG, &lsb);

      /* Get MSB */
      status |= Ctrl_WriteRegister(I2c, MCNSSD_SEL_REG, 0x0005);
      status |= Ctrl_ReadRegister(I2c, MCNSSD_REG, &msb);

      tmp = msb << 16;
      tmp |= lsb;
      break;

    case CW_COUNT:
      /* CW_COUNT = MSB * 2^16 +  LSB */
      /* Get LSB */
      status |= Ctrl_WriteRegister(I2c, MCNSSD_SEL_REG, 0x0000);
      status |= Ctrl_ReadRegister(I2c, MCNSSD_REG, &lsb);
      /* Get MSB */
      status |= Ctrl_WriteRegister(I2c, MCNSSD_SEL_REG, 0x0001);
      status |= Ctrl_ReadRegister(I2c, MCNSSD_REG, &msb);

      tmp = msb << 16;
      tmp |= lsb;
      break;

    case CORR_BITS:
      status |= Ctrl_ReadRegister(I2c, NCBL_REG, &lsb);
      status |= Ctrl_ReadRegister(I2c, NCBH_REG, &msb);
      msb &= 0x00FF; /* Only 8bit is valid */

      tmp = msb << 16;
      tmp |= lsb;
      break;
    
    default:
      tmp = 0;
      break;
  }

 *dataPtr = tmp;

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodBer - MXL_DEMOD_BER_UNCODED_BER_CER_REQ
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/19/2009
--|                 3/02/2010
--|                 8/13/2010
--|
--| DESCRIPTION   : This function returns BER, CER, and Uncoded BER, and
--|                 supports either float or integer calculation depending on
--|                 conditional compile (__MXL_INTEGER_CALC_STATISTICS__)
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

#ifdef __MXL_INTEGER_CALC_STATISTICS__
MXL_STATUS MxL_GetDemodBer(PMXL_DEMOD_BER_INFO_T BerInfoPtr) 
{
  UINT8 status = MXL_TRUE;
  UINT32 cw_corr_count;
  UINT32 cw_err_count;
  UINT32 cw_count;
  UINT32 cw_unerr_count;
  UINT32 corr_bits;        
  UINT32 err_mpeg;
  UINT16 received_mpeg;

  UINT16 kcer = 0, kber = 0, kuber = 0;
  UINT16 RegData, AnnexType, QamType;
  UINT8  Idx;

  /* Find a database id of the target device for statistics */
  for (Idx = 0; Idx < MAX_241SF_DEVICES; Idx++)
  {
    if (AccStatCounter[Idx].I2c.SlaveAddr == BerInfoPtr->I2c.SlaveAddr) break;
  }

  if (Idx == MAX_241SF_DEVICES) return MXL_FALSE;

  /* Check Annex Type and QAM TYPE */
  status |= Ctrl_ReadRegister(BerInfoPtr->I2c, QAM_ANNEX_TYPE_REG, &RegData); 
  AnnexType = RegData & ANNEX_A_TYPE;    /* Bit 10 of Reg 0x8001 gives ANNEX Type */
  QamType = RegData & QAM_TYPE;          /* Bit <2:0> of Reg 0x8001 gives QAM Type */

  /* Clear the internal HW counters to avoid overflow */
  status |= Ctrl_WriteRegister(BerInfoPtr->I2c, STAMP_REG, 0x0055);
  
  GetMCNSSD(BerInfoPtr->I2c, CW_CORR_COUNT, &cw_corr_count);
  GetMCNSSD(BerInfoPtr->I2c, CW_ERR_COUNT, &cw_err_count);
  GetMCNSSD(BerInfoPtr->I2c, CW_COUNT, &cw_count);
  GetMCNSSD(BerInfoPtr->I2c, CORR_BITS, &corr_bits);

  cw_unerr_count = cw_count - cw_corr_count - cw_err_count;

  status |= Ctrl_ReadRegister(BerInfoPtr->I2c, FRCNT_REG, &RegData); 

  /* ERR_MPEG */
  if (AnnexType == ANNEX_A_TYPE) 
  {
    received_mpeg = 0xFFFF - RegData;
    err_mpeg = cw_err_count;
  }
  else
  {
    received_mpeg = RegData;

    status |= Ctrl_ReadRegister(BerInfoPtr->I2c, MEF_REG, &RegData); 
    err_mpeg = RegData;
  }

  /* Update Statistic counter */
  AccStatCounter[Idx].AccCwCorrCount += cw_corr_count;
  AccStatCounter[Idx].AccCwErrCount += cw_err_count;
  AccStatCounter[Idx].AccCwUnerrCount += cw_unerr_count;
  AccStatCounter[Idx].AccCorrBits += corr_bits;
  AccStatCounter[Idx].AccErrMpeg += err_mpeg;
  AccStatCounter[Idx].AccReceivedMpeg += received_mpeg;
  AccStatCounter[Idx].AccCwReceived = AccStatCounter[Idx].AccCwCorrCount + \
                                      AccStatCounter[Idx].AccCwErrCount  + \
                                      AccStatCounter[Idx].AccCwUnerrCount;

  /* Calculate BER, CER, and Uncoded BER */
  if (AnnexType == ANNEX_A_TYPE) 
  {
    if (QamType == MXL_QAM64)
    {
      kcer = 43844;
      kber = 263;
      kuber = 27;
    }
    else
    {
      kcer = 32894;
      kber = 197;
      kuber = 20;
    }
  }
  else
  {
    if (QamType == MXL_QAM64)
    {
      kcer = 35129;
      kber = 411;
      kuber = 39;
    }
    else
    {
      kcer = 24418;
      kber = 286;
      kuber = 27;
    }
  }

  /* Calculate CER, BER, and Uncoded BER */
  BerInfoPtr->CER = kcer * cw_err_count;
  BerInfoPtr->BER = kber * err_mpeg;
  BerInfoPtr->UncodedBER = kuber * corr_bits;

  MxL_DLL_DEBUG0("CER = 0x%08x\n", (UINT32)BerInfoPtr->CER);
  MxL_DLL_DEBUG0("BER = 0x%08x\n", (UINT32)BerInfoPtr->BER);
  MxL_DLL_DEBUG0("Uncoded BER = 0x%08x\n", (UINT32)BerInfoPtr->UncodedBER);

  return (MXL_STATUS)status;
}
#else
MXL_STATUS MxL_GetDemodBer(PMXL_DEMOD_BER_INFO_T BerInfoPtr) 
{
  UINT8 status = MXL_TRUE;
  UINT32 cw_corr_count;
  UINT32 cw_err_count;
  UINT32 cw_count;
  UINT32 cw_unerr_count;
  UINT32 corr_bits;        
  UINT32 err_mpeg;
  UINT16 received_mpeg;

  UINT16 RegData, AnnexType;
  UINT8  Idx;

  /* Find a database id of the target device for statistics */
  for (Idx = 0; Idx < MAX_241SF_DEVICES; Idx++)
  {
    if (AccStatCounter[Idx].I2c == BerInfoPtr->I2c) break;
  }

  if (Idx == MAX_241SF_DEVICES) return MXL_FALSE;

  /* Check Annex Type and QAM TYPE */
  status |= Ctrl_ReadRegister(BerInfoPtr->I2c, QAM_ANNEX_TYPE_REG, &RegData); 
  AnnexType = RegData & ANNEX_A_TYPE;    /* Bit 10 of Reg 0x8001 gives ANNEX Type */

  /* Clear the internal HW counters to avoid overflow */
  status |= Ctrl_WriteRegister(BerInfoPtr->I2c, STAMP_REG, 0x0055);
  
  GetMCNSSD(BerInfoPtr->I2c, CW_CORR_COUNT, &cw_corr_count);
  GetMCNSSD(BerInfoPtr->I2c, CW_ERR_COUNT, &cw_err_count);
  GetMCNSSD(BerInfoPtr->I2c, CW_COUNT, &cw_count);
  GetMCNSSD(BerInfoPtr->I2c, CORR_BITS, &corr_bits);    

  cw_unerr_count = cw_count - cw_corr_count - cw_err_count;

  status |= Ctrl_ReadRegister(BerInfoPtr->I2c, FRCNT_REG, &RegData); 

  /* ERR_MPEG */
  if (AnnexType == ANNEX_A_TYPE) 
  {
    received_mpeg = 0xFFFF - RegData;
    err_mpeg = cw_err_count;
  }
  else
  {
    received_mpeg = RegData;

    status |= Ctrl_ReadRegister(BerInfoPtr->I2c, MEF_REG, &RegData); 
    err_mpeg = RegData;
  }

  /* Update Statistic counter */
  AccStatCounter[Idx].AccCwCorrCount += cw_corr_count;
  AccStatCounter[Idx].AccCwErrCount += cw_err_count;
  AccStatCounter[Idx].AccCwUnerrCount += cw_unerr_count;
  AccStatCounter[Idx].AccCorrBits += corr_bits;
  AccStatCounter[Idx].AccErrMpeg += err_mpeg;
  AccStatCounter[Idx].AccReceivedMpeg += received_mpeg;
  AccStatCounter[Idx].AccCwReceived = AccStatCounter[Idx].AccCwCorrCount + \
                                      AccStatCounter[Idx].AccCwErrCount  + \
                                      AccStatCounter[Idx].AccCwUnerrCount;

  /* Check boundary case */
  if (AccStatCounter[Idx].AccCwReceived == 0)
  {
    BerInfoPtr->CER = 0.0;
  }
  else
  {
    /* Calculate CER */
    BerInfoPtr->CER = (REAL64)AccStatCounter[Idx].AccCwErrCount / AccStatCounter[Idx].AccCwReceived;
  }
  
  /* Calculate BER and Uncoded BER */
  if (AnnexType == ANNEX_A_TYPE) 
  {
    if (AccStatCounter[Idx].AccReceivedMpeg == 0) BerInfoPtr->BER = 0.0;
    else BerInfoPtr->BER = (9.0 * (REAL64)AccStatCounter[Idx].AccErrMpeg) / (188.0 * 8.0 * (REAL64)AccStatCounter[Idx].AccReceivedMpeg);

    if (AccStatCounter[Idx].AccCwReceived == 0) BerInfoPtr->UncodedBER = 0.0;
    else BerInfoPtr->UncodedBER = (REAL64)AccStatCounter[Idx].AccCorrBits / ((REAL64)AccStatCounter[Idx].AccCwReceived * 204.0 * 8.0);
  }
  else
  {
    if (AccStatCounter[Idx].AccReceivedMpeg == 0) BerInfoPtr->BER = 0.0;
    else BerInfoPtr->BER = (10.0 * (REAL64)AccStatCounter[Idx].AccErrMpeg) / (188.0 * 8.0 * (REAL64)AccStatCounter[Idx].AccReceivedMpeg);

    if (AccStatCounter[Idx].AccCwReceived == 0) BerInfoPtr->UncodedBER = 0.0;
    else BerInfoPtr->UncodedBER = (REAL64)AccStatCounter[Idx].AccCorrBits / ((REAL64)AccStatCounter[Idx].AccCwReceived * 128.0 * 7.0);
  }

  MxL_DLL_DEBUG0("CER = %f\n", BerInfoPtr->CER);    
  MxL_DLL_DEBUG0("BER = %f\n", BerInfoPtr->BER);    
  MxL_DLL_DEBUG0("Uncoded BER = %f\n", BerInfoPtr->UncodedBER);    

  return (MXL_STATUS)status;       
}
#endif

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodStatisticCounters - MXL_DEMOD_STAT_COUNTERS_REQ
--| 
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/24/2009
--|
--| DESCRIPTION   : It reads back statistic counters
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetDemodStatisticCounters(PMXL_DEMOD_STAT_COUNT_T StatCountPtr)
{
  MXL_STATUS status = MXL_FALSE;
  UINT8 i;

  /* Search for device's statistics database */
  for (i = 0; i < MAX_241SF_DEVICES; i++)
  {
    if (AccStatCounter[i].I2c.SlaveAddr == StatCountPtr->I2c.SlaveAddr) break;
  }

  if (i < MAX_241SF_DEVICES)
  {
    /* Accumulated counter for corrected code word */
    StatCountPtr->AccCwCorrCount = AccStatCounter[i].AccCwCorrCount;

    /* Accumulated counter for uncorrected code word */
    StatCountPtr->AccCwErrCount = AccStatCounter[i].AccCwErrCount;

    /* Accumulated total received code words */
    StatCountPtr->AccCwReceived = AccStatCounter[i].AccCwReceived;
    
    /* Accumulated counter for code words received*/
    StatCountPtr->AccCwUnerrCount = AccStatCounter[i].AccCwUnerrCount;

    /* Accumulated counter for corrected bits*/
    StatCountPtr->AccCorrBits = AccStatCounter[i].AccCorrBits;

    /* Accumulated counter for erred mpeg frames*/
    StatCountPtr->AccErrMpeg = AccStatCounter[i].AccErrMpeg;

    /* Accumulated counter for received mpeg frames*/
    StatCountPtr->AccReceivedMpeg = AccStatCounter[i].AccReceivedMpeg;     

    status = MXL_TRUE;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_ResetDemodStatCounters - MXL_DEMOD_RESET_STAT_COUNTER_CFG
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/25/2009
--|
--| DESCRIPTION   : It resets the statistic variales that shall be used to calculate 
--|                 BER, CER, Uncoded BER.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_ResetDemodStatCounters(PMXL_RESET_COUNTER_T ResetCountPtr)
{
  MXL_STATUS status = MXL_FALSE;
  UINT8 i;

  /* Find out device's statistics database */
  for (i = 0; i < MAX_241SF_DEVICES; i++)
  {
    if (AccStatCounter[i].I2c.SlaveAddr == ResetCountPtr->I2c.SlaveAddr) break;
  }

  if (i < MAX_241SF_DEVICES)
  {
    AccStatCounter[i].AccCwCorrCount = 0;
    AccStatCounter[i].AccCwErrCount = 0;
    AccStatCounter[i].AccCwUnerrCount = 0;
    AccStatCounter[i].AccCwReceived = 0;
    AccStatCounter[i].AccCorrBits = 0;
    AccStatCounter[i].AccErrMpeg = 0;
    AccStatCounter[i].AccReceivedMpeg = 0;

    status = MXL_TRUE;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodAnnexQamType - MXL_DEMOD_ANNEX_QAM_TYPE_REQ
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 06/17/2009
--|
--| DESCRIPTION   : Get Annex (B or A) and Qam type
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetDemodAnnexQamType(PMXL_DEMOD_ANNEXQAM_INFO_T AnnexQamTypePtr)
{
  MXL_STATUS status;
  UINT16 RegData;

  status = Ctrl_ReadRegister(AnnexQamTypePtr->I2c, QAM_ANNEX_TYPE_REG, &RegData); 

  /* Annex type */
  AnnexQamTypePtr->AnnexType = (MXL_ANNEX_TYPE_E)((RegData & 0x0400) >> 10);

  /* QAM type */
  AnnexQamTypePtr->QamType = (MXL_QAM_TYPE_E)(RegData & 0x0007);

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodCarrierOffset - MXL_DEMOD_CARRIER_OFFSET_REQ
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 06/17/2009, 06/02/2010
--|
--| DESCRIPTION   : Get carrier offset
--|                 before reading Carrier Offset, 
--|                 addr:0x803E, data:0x0087 need to be written, and
--|                 supports either float or integer calculation depending on
--|                 conditional compile (__MXL_INTEGER_CALC_STATISTICS__)
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

#ifdef __MXL_INTEGER_CALC_STATISTICS__
MXL_STATUS MxL_GetDemodCarrierOffset(PMXL_DEMOD_CARRIEROFFSET_INFO_T CarrierOffsetPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 sr = 0;
  UINT16 regData;
  
  /* Before readback, register 0x803E needs to be written by data 0x0087  */
  status |= Ctrl_WriteRegister(CarrierOffsetPtr->I2c, CARRIER_OFFSET_REG, 0x0087);

  switch(CarrierOffsetPtr->SymbolRate)
  {
    case SYM_RATE_ANNEX_A_6_89MHz:
      sr = 13142;
      break;

    case SYM_RATE_ANNEX_B64_5_0569MHz:
      sr = 9645;
      break;

    case SYM_RATE_ANNEX_B256_5_3605MHz:
      sr = 10224;
      break;

    case SYM_RATE_OOB_0_772MHz:
      sr = 1377;
      break;

    case SYM_RATE_OOB_1_024MHz:
      sr = 1953;
      break;

    case SYM_RATE_OOB_1_544MHz:
      sr = 2945;
      break;

    default:
      status |= MXL_FALSE;
      break;
  }

  /* Read the current settings. */
  status |= Ctrl_ReadRegister(CarrierOffsetPtr->I2c, CARRIER_OFFSET_RB_REG, &regData);

  /* 1E9 * Carrier Offset = SR * signed(X) [MHz] */
  CarrierOffsetPtr->CarrierOffset = sr * (SINT16)regData;
  
  return (MXL_STATUS)status;
}
#else
MXL_STATUS MxL_GetDemodCarrierOffset(PMXL_DEMOD_CARRIEROFFSET_INFO_T CarrierOffsetPtr)
{
  MXL_DEMOD_ANNEXQAM_INFO_T AnnexQamType;
  UINT8 status = MXL_TRUE;
  UINT8 byPass;
  UINT16 regData;
  REAL32 SymbolRate = 0;
  UINT32 nominalSymRate = 0;

  /* Before readback, register 0x803E needs to be written by data 0x0087  */
  status |= Ctrl_WriteRegister(CarrierOffsetPtr->I2c, CARRIER_OFFSET_REG, 0x0087);
  
  /* Check Annex and Qam type */
  AnnexQamType.I2c = CarrierOffsetPtr->I2c;
  status |= MxL_GetDemodAnnexQamType(&AnnexQamType);

  /* Read the current settings. */
  status |= Ctrl_ReadRegister(CarrierOffsetPtr->I2c, SYMBOL_RATE_RESAMP_BANK_REG, &regData);

  /* Save bypass mode*/
  byPass = (UINT8)regData & 0x0010;

  /* Config Bank <2:0> */
  if (AnnexQamType.AnnexType == ANNEX_A)
  {
    /* Annex A bank = 0 */
    regData &= 0xFFF8;
  }
  else
  {
    if (AnnexQamType.QamType == MXL_QAM64)
    {
      /* Annex B and 64QAM = 1 */
      regData &= 0xFFF9;
    }
    else
    {
      /* Annex B and 256QAM = 2 */
      regData &= 0xFFFA;
    }
  }
  status |= Ctrl_WriteRegister(CarrierOffsetPtr->I2c, SYMBOL_RATE_RESAMP_BANK_REG, regData);

  /* SYMBOL_RATE_RATE_HI_REG_RB <2:0> */
  status |= Ctrl_ReadRegister(CarrierOffsetPtr->I2c, SYMBOL_RATE_RATE_HI_REG_RB, &regData);
  nominalSymRate = (regData & 0x0003) << 24;

  /* SYMBOL_RATE_RATE_MID_HI_REG_RB <7:0> */
  status |= Ctrl_ReadRegister(CarrierOffsetPtr->I2c, SYMBOL_RATE_RATE_MID_HI_REG_RB, &regData);
  nominalSymRate |= (regData & 0x00FF) << 16;

  /* SYMBOL_RATE_RATE_MID_LO_REG_RB <7:0> */
  status |= Ctrl_ReadRegister(CarrierOffsetPtr->I2c, SYMBOL_RATE_RATE_MID_LO_REG_RB, &regData);
  nominalSymRate |= (regData & 0x00FF) << 8;

  /* SYMBOL_RATE_RATE_LO_REG_RB <7:0> */
  status |= Ctrl_ReadRegister(CarrierOffsetPtr->I2c, SYMBOL_RATE_RATE_LO_REG_RB, &regData);
  nominalSymRate |= (regData & 0x00FF);

  if (byPass == MXL_ENABLE)
    SymbolRate = (REAL32)(nominalSymRate * 4 / (1 << 24));
  else
    SymbolRate = (REAL32)(nominalSymRate / (REAL32)((1 << 24) * 0.75));

  if (0 != SymbolRate) SymbolRate = (REAL32)(9.5 / SymbolRate);
  else status |= MXL_FALSE;

  /* Read the current settings. */
  status |= Ctrl_ReadRegister(CarrierOffsetPtr->I2c, CARRIER_OFFSET_RB_REG, &regData);

  /* CarrierOffset = SymbolRate * Singed(regData) * 2^(-N+1+log2(1/16)) */
  /* N = number of bits, therefore, N = 16  */
  CarrierOffsetPtr->CarrierOffset = SymbolRate * (SINT16)regData;

  /* pow (2, -19) */
  CarrierOffsetPtr->CarrierOffset *= (REAL64)1 / (1 << 19); 
  
  return (MXL_STATUS)status;
}
#endif

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodInterleaverDepth - MXL_DEMOD_INTERLEAVER_DEPTH_REQ
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 06/18/2009
--|
--| DESCRIPTION   : Get Interleaver Depth I and J
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetDemodInterleaverDepth(PMXL_INTERDEPTH_INFO_T InterDepthPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 regData, annexType;
  PINTERLEAVER_LOOKUP_INFO_T InterDepthLoopUpTable = MxL_InterDepthLookUpTable;

  /* Read the current Annex Type <10> */
  status |= Ctrl_ReadRegister(InterDepthPtr->I2c, QAM_ANNEX_TYPE_REG, &annexType);
  annexType &= 0x0400;

  /* before readback, register 0x80A3 needs to be written by data 0 <3:0> */
  status |= Ctrl_ReadRegister(InterDepthPtr->I2c, INTERLEAVER_DEPTH_REG, &regData);

  /* written by data 0 <3:0> */
  regData &= 0xFFF0;
  status |= Ctrl_WriteRegister(InterDepthPtr->I2c, INTERLEAVER_DEPTH_REG, regData);
  
  /* Read the current settings. */
  status |= Ctrl_ReadRegister(InterDepthPtr->I2c, INTERLEAVER_DEPTH_REG, &regData);

  /* Interleaver Depth I, J <6:3>
   * regData = Control word(4bits) becomes 
   * the address of InterleaverDepth LookUp Table
   */
  regData = (regData >> 3) & 0x000F;

  if (annexType == ANNEX_B) /* Annex_B */
  {
    InterDepthPtr->InterDepthI = InterDepthLoopUpTable[regData].interDepthI;
    InterDepthPtr->InterDepthJ = InterDepthLoopUpTable[regData].interDepthJ;
  }
  else           /* Annex_A */
  {
    InterDepthPtr->InterDepthI = 12;
    InterDepthPtr->InterDepthJ = 17;
  }

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodEqualizerFilter - MXL_DEMOD_EQUALIZER_FILTER_REQ
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 06/18/2009, 06/02/2010
--|
--| DESCRIPTION   : Get Equalizer filter, and supports either float or
--|                 integer calculation depending on conditional compile
--|                 (__MXL_INTEGER_CALC_STATISTICS__)
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

#ifdef __MXL_INTEGER_CALC_STATISTICS__
MXL_STATUS MxL_GetDemodEqualizerFilter(PMXL_DEMOD_EQUALFILTER_INFO_T EqualizerFilterPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 counter = 0, tmpValue = 0;
  UINT16 regData = 0;
  SINT16 tmpData = 0;
  
  /* EXTENDED SPACE ADDRESS <7:0>
   * Address auto increment enable <8>
   * FFE filer start 00 to FF
   */
  status |= Ctrl_WriteRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_REG, 0x0100);

  /* Readback data
   * Oddth data = X, eventh data = Y
   * 1E9* FFE Tap = (signed(X) * 15259) + (j * signed(Y) * 15259)
   */
  while (counter < 16)
  {
    status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_DATA_RB_REG, &regData);
    EqualizerFilterPtr->FfeInfo[counter] = (SINT16)regData * 15259;

    counter++;
  }

  /* Spur filer start 20 to 3F */
  status |= Ctrl_WriteRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_REG, 0x0120);

  /* Readback data
   * Oddth data = X, eventh data = Y
   * 1E9* Spur Tap = (signed(X) * 488281) + (j * signed(Y) * 488281)
   */
  counter = 0;
  while (counter < 32)
  {
    status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_DATA_RB_REG, &regData);
    regData &= 0x0FFF;
    
    /* Check signed or unsigned */
    if (regData & 0x0800) tmpData = (SINT16)(0xF000 | regData);
    else tmpData = (SINT16)regData;

    EqualizerFilterPtr->SpurInfo[counter] = tmpData * 488281;

    counter++;
  }

  /* DFE filer start 40 to 77  */
  status |= Ctrl_WriteRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_REG, 0x0140);

  /* Readback data
   * Oddth data = X, eventh data = Y
   * 1E9* DFE Tap = (signed(X) * 15259) + (j * signed(Y) * 15259)
   */
  counter = 0;
  while (counter < 56)
  {
    status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_DATA_RB_REG, &regData);
    EqualizerFilterPtr->DfeInfo[counter] = (SINT16)regData * 15259;

    counter++;
  }

  /* Main Tap Location */
  /* Reading register 0x8026, bits (8-9), holds the main tap location with deviation of 5 */
  status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, PHY_EQUALIZER_FFE_FILTER_REGISTER, &regData);
  tmpValue = ((regData >> 8) & 0x0003);

  switch(tmpValue)
  {
    case 0:
      EqualizerFilterPtr->DsEqMainLocation = FFE_MAIN_TAP_LOCATION_1;
      break;

    case 1:
      EqualizerFilterPtr->DsEqMainLocation = FFE_MAIN_TAP_LOCATION_2;
      break;

    case 2:
      EqualizerFilterPtr->DsEqMainLocation = FFE_MAIN_TAP_LOCATION_3;
      break;

    case 3:
      EqualizerFilterPtr->DsEqMainLocation = FFE_MAIN_TAP_LOCATION_4;
      break;

    default:
      MxL_DLL_DEBUG0("Unsupported main tap location.\n");
      status |= MXL_FALSE;
      break;
  }

  /* Number of DFE Taps */
  /* Reading register 0x8027, bits (0-4) +1, holds the number of reverse taps */
  status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, PHY_EQUALIZER_DFE_FILTER_REGISTER, &regData);
  tmpValue = regData & 0x001F;
  EqualizerFilterPtr->DsEqDfeTapNum = (UINT8)(tmpValue + 1);

  return (MXL_STATUS)status;
}
#else
MXL_STATUS MxL_GetDemodEqualizerFilter(PMXL_DEMOD_EQUALFILTER_INFO_T EqualizerFilterPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 counter = 0, tmpValue = 0;
  UINT16 regData = 0;
  SINT16 tmpData = 0;
  
  /* EXTENDED SPACE ADDRESS <7:0>
   * Address auto increment enable <8>
   * FFE filer start 00 to FF
   */
  status |= Ctrl_WriteRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_REG, 0x0100);

  /* Readback data
   * Oddth data = X, eventh data = Y
   * FFE Tap = signed(X)*2^(-n+1+log2(1/2)) + j*signed(Y)*2^(-n+1+log2(1/2))
   * n = 16 for FFE
   */
  while (counter < 16)
  {
    status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_DATA_RB_REG, &regData);
    EqualizerFilterPtr->FfeInfo[counter] = (REAL32)((SINT16)regData * ((REAL32)1 / (1 << 16)));

    counter++;
  }

  /* Spur filer start 20 to 3F */
  status |= Ctrl_WriteRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_REG, 0x0120);

  /* Readback data
   * Oddth data = X, eventh data = Y
   * Spur Tap = signed(X)*2^(-n+1+log2(1)) + j*signed(Y)*2^(-n+1+log2(1))
   * n = 12 for FFE
   */
  counter = 0;
  while (counter < 32)
  {
    status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_DATA_RB_REG, &regData);
    regData &= 0x0FFF;
    
    /* Check signed or unsigned */
    if (regData & 0x0800) tmpData = (SINT16)(0xF000 | regData);
    else tmpData = (SINT16)regData;

    EqualizerFilterPtr->SpurInfo[counter] = (REAL32)(tmpData * ((REAL32)1 / (1 << 11)));

    counter++;
  }

  /* DFE filer start 40 to 77  */
  status |= Ctrl_WriteRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_REG, 0x0140);

  /* Readback data
   * Oddth data = X, eventh data = Y
   * DFE Tap = signed(X)*2^(-n+1+log2(1/2)) + j*signed(Y)*2^(-n+1+log2(1/2))
   * n = 16 for DEF
   */
  counter = 0;
  while (counter < 56)
  {
    status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, EQUALIZER_FILTER_DATA_RB_REG, &regData);
    EqualizerFilterPtr->DfeInfo[counter] = (REAL32)((SINT16)regData * ((REAL32)1 / (1 << 16)));

    counter++;
  }

  /* Main Tap Location */
  /* Reading register 0x8026, bits (8-9), holds the main tap location with deviation of 5 */
  status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, PHY_EQUALIZER_FFE_FILTER_REGISTER, &regData);
  tmpValue = ((regData >> 8) & 0x0003);

  switch(tmpValue)
  {
    case 0:
      EqualizerFilterPtr->DsEqMainLocation = FFE_MAIN_TAP_LOCATION_1;
      break;

    case 1:
      EqualizerFilterPtr->DsEqMainLocation = FFE_MAIN_TAP_LOCATION_2;
      break;

    case 2:
      EqualizerFilterPtr->DsEqMainLocation = FFE_MAIN_TAP_LOCATION_3;
      break;

    case 3:
      EqualizerFilterPtr->DsEqMainLocation = FFE_MAIN_TAP_LOCATION_4;
      break;

    default:
      MxL_DLL_DEBUG0("Unsupported main tap location.\n");
      status |= MXL_FALSE;
      break;
  }

  /* Number of DFE Taps */
  /* Reading register 0x8027, bits (0-4) +1, holds the number of reverse taps */
  status |= Ctrl_ReadRegister(EqualizerFilterPtr->I2c, PHY_EQUALIZER_DFE_FILTER_REGISTER, &regData);
  tmpValue = regData & 0x001F;
  EqualizerFilterPtr->DsEqDfeTapNum = (UINT8)(tmpValue + 1);

  return (MXL_STATUS)status;
}
#endif

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodTimingOffset - MXL_DEMOD_TIMING_OFFSET_REQ
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 07/21/2009
--|
--| DESCRIPTION   : Get timing-offset of the receiver
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetDemodTimingOffset(PMXL_DEMOD_TIMINGOFFSET_INFO_T TimingOffsetPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 bankCfgData = 0;
  UINT16 regData;
  SINT16 godardAcc;
  UINT32 tmpRate = 0;
  MXL_DEMOD_ANNEXQAM_INFO_T AnnexQamType;

  /* Readback current Annex and QAM type first */
  AnnexQamType.I2c = TimingOffsetPtr->I2c;
  status = MxL_GetDemodAnnexQamType(&AnnexQamType);

  switch(AnnexQamType.AnnexType)
  {
  case ANNEX_B:
    switch(AnnexQamType.QamType)
    {
    case MXL_QAM64:
      bankCfgData = 1;
      break;

    case MXL_QAM256:
      bankCfgData = 2;
      break;

    default:
      return MXL_FALSE;
    }
    break;

  case ANNEX_A:
    bankCfgData = 0;
    break;
  }

  /* Config bank <2:0> */
  status |= Ctrl_ReadRegister(TimingOffsetPtr->I2c, RESAMP_BANK_REG, &regData);
  regData = ((regData & 0xFFF8) | bankCfgData);
  status |= Ctrl_WriteRegister(TimingOffsetPtr->I2c, RESAMP_BANK_REG, regData);

  /* Resample readback: High<10:0>, Low<15:0> */
  status |= Ctrl_ReadRegister(TimingOffsetPtr->I2c, RATE_RESAMP_RATE_MID_HI_REG, &regData);
  tmpRate = (regData & 0x07FF) << 16;
  status |= Ctrl_ReadRegister(TimingOffsetPtr->I2c, RATE_RESAMP_RATE_MID_LO_REG, &regData);
  tmpRate = ((tmpRate & 0xFFFF0000) | regData);

  /* Calculate timing offset (ppm unit) */
  status |= Ctrl_ReadRegister(TimingOffsetPtr->I2c, GODARD_ACC_REG, (UINT16*)&godardAcc);

  if (0 != tmpRate)
  {
    UINT64 tmpTimingOffset = (UINT64)godardAcc * 1000000;
    do_div(tmpTimingOffset, tmpRate);
    TimingOffsetPtr->TimingOffset = tmpTimingOffset;
  }
  else
  {
    status |= MXL_FALSE;
  }

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetDemodReTuneIndicator - MXL_DEMOD_RETUNE_INDICATOR_REQ
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 04/13/2011
--|
--| DESCRIPTION   : Get retune indicator status
--|                 When both FEC and MPEG locked, calling this API returns
--|                 retune required or not
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS MxL_GetDemodReTuneIndicator(PMXL_RETUNE_IND_T ParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 regData;

  /* Before readback, register 0x803E needs to be written by data 0x0086  */
  status = Ctrl_WriteRegister(ParamPtr->I2c, SNR_EXT_SPACE_ADDR_REG, 0x0086);

  status |= Ctrl_ReadRegister(ParamPtr->I2c, SNR_EXT_SPACE_DATA_REG, &regData);

  ParamPtr->ReTuneInd = (regData < RETUNE_INDICATOR_THRESHOLD) ? MXL_TRUE : MXL_FALSE;

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetTunerLockStatus - MXL_TUNER_LOCK_STATUS_REQ
--|
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/25/2009
--|                 11/10/2009
--|                 3/02/2010
--|
--| DESCRIPTION   : This function returns Tuner, AGC Lock, and
--|                 Tuner Done status after tuning.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetTunerLockStatus(PMXL_TUNER_LOCK_STATUS_T TunerLockStatusPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 lock;

  status = Ctrl_ReadRegister(TunerLockStatusPtr->I2c, RF_LOCK_STATUS_REG, &lock);

  if ((lock & TUNER_LOCKED) == TUNER_LOCKED)
  {
    TunerLockStatusPtr->TunerLockStatus = MXL_LOCKED;
  }
  else
  {
    TunerLockStatusPtr->TunerLockStatus = MXL_UNLOCKED;
  }

  /* AGC Lock <2> */
  status |= Ctrl_ReadRegister(TunerLockStatusPtr->I2c, AGC_LOCK_STATUS_REG, &lock);

  if ((lock & AGC_LOCKED) == AGC_LOCKED)
  {
    TunerLockStatusPtr->AgcLockStatus = MXL_LOCKED;
  }
  else
  {
    TunerLockStatusPtr->AgcLockStatus = MXL_UNLOCKED;
  }

  /* Tuner Done <3> */
  if ((lock & TUNER_DONE) == TUNER_DONE)
  {
    TunerLockStatusPtr->TunerDoneStatus = MXL_LOCKED;
  }
  else
  {
    TunerLockStatusPtr->TunerDoneStatus = MXL_UNLOCKED;
  }

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetTunerAgcSettings - MXL_TUNER_AGC_SETTINGS_REQ
--|
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/25/2009
--|
--| DESCRIPTION   : This function returns the current AGC settings.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxL_GetTunerAgcSettings(PMXL_AGC_T AgcParamPtr) 
{
  MXL_STATUS status;
  UINT16 control;

  status = Ctrl_ReadRegister(AgcParamPtr->I2c, DIG_HALT_GAIN_CTRL_REG, &control);
  AgcParamPtr->FreezeAgcGainWord = (MXL_BOOL)((control & FREEZE_AGC_GAIN_WORD) >> 3);

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL_GetTunerRxPower - MXL_TUNER_RF_RX_PWR_REQ
--|
--| AUTHOR        : Brenndon Lee
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/25/2009
--|                 6/02/2010
--|
--| DESCRIPTION   : This function returns RF input power in dBm, and
--|                 supports either float or integer calculation depending on
--|                 conditional compile (__MXL_INTEGER_CALC_STATISTICS__)
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

#ifdef __MXL_INTEGER_CALC_STATISTICS__
MXL_STATUS MxL_GetTunerRxPower(PMXL_TUNER_RX_PWR_T TunerRxPwrPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 RegRb, RfpinData;

  /* Enable RF input power readback */
  status |= Ctrl_WriteRegister(TunerRxPwrPtr->I2c, RF_PIN_RB_EN_REG, ENABLE_RFPIN_RB);

  /* RF power readback 0x0018 <7:0>, 0x0019 <2:0> */
  status |= Ctrl_ReadRegister(TunerRxPwrPtr->I2c, DIG_RF_PIN_RB_LO_REG, &RegRb);
  RfpinData = RegRb & 0x00FF;

  status |= Ctrl_ReadRegister(TunerRxPwrPtr->I2c, DIG_RF_PIN_RB_HI_REG, &RegRb);
  RfpinData |= ((RegRb & 0x0007) << 8);

  /* Calculate */
  TunerRxPwrPtr->RxPwr = RfpinData * 125; 
  TunerRxPwrPtr->RxPwr -= 120000;

  return (MXL_STATUS)status;
}
#else
MXL_STATUS MxL_GetTunerRxPower(PMXL_TUNER_RX_PWR_T TunerRxPwrPtr)
{
  UINT8 status = MXL_TRUE;
  UINT16 RegRb, RfpinData;

  /* Enable RF input power readback */
  status |= Ctrl_WriteRegister(TunerRxPwrPtr->I2c, RF_PIN_RB_EN_REG, ENABLE_RFPIN_RB);

  /* RF power readback 0x0018 <7:0>, 0x0019 <2:0> */
  status |= Ctrl_ReadRegister(TunerRxPwrPtr->I2c, DIG_RF_PIN_RB_LO_REG, &RegRb);
  RfpinData = RegRb & 0x00FF;

  status |= Ctrl_ReadRegister(TunerRxPwrPtr->I2c, DIG_RF_PIN_RB_HI_REG, &RegRb);
  RfpinData |= ((RegRb & 0x0007) << 8);

  /* Get Integer part */
  TunerRxPwrPtr->RxPwr = (REAL32)(RfpinData >> 3);
 
  /* Calculate fractional part */
  RfpinData &= 0x7;  
 
  if (RfpinData & 0x1) TunerRxPwrPtr->RxPwr += 0.125;
 
  RfpinData >>= 1;
 
  if (RfpinData & 0x1) TunerRxPwrPtr->RxPwr += 0.25;
 
  RfpinData >>= 1;
 
  if (RfpinData & 0x1) TunerRxPwrPtr->RxPwr += 0.5;
 
  TunerRxPwrPtr->RxPwr -= 120.0;

  return (MXL_STATUS)status;
}
#endif

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare_API_ConfigDevice 
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : The general device configuration shall be handled 
--|                 through this API
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxLWare_API_ConfigDevice(MXL_CMD_TYPE_E CmdType, void *ParamPtr)
{
  MXL_STATUS status = MXL_TRUE;

  MxL_DLL_DEBUG0("MxLWare_API_ConfigDevice - %d\n", CmdType);

  switch (CmdType)
  {
    case MXL_DEV_SOFT_RESET_CFG:
      status = MxL_ConfigDevReset((PMXL_RESET_CFG_T)ParamPtr);
      break;

    case MXL_DEV_XTAL_SETTINGS_CFG:
      status = MxL_ConfigDevXtalSettings((PMXL_XTAL_CFG_T)ParamPtr);
      break;

    case MXL_DEV_POWER_MODE_CFG:
      status = MxL_ConfigDevPowerSavingMode((PMXL_PWR_MODE_CFG_T)ParamPtr);
      break;

    case MXL_DEV_OVERWRITE_DEFAULT_CFG:
      status = MxL_ConfigDevOverwriteDefault((PMXL_OVERWRITE_DEFAULT_CFG_T)ParamPtr);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare_API_GetDeviceStatus 
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : The general device inquiries shall be handled 
--|                 through this API
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxLWare_API_GetDeviceStatus(MXL_CMD_TYPE_E CmdType, void *ParamPtr)
{
  MXL_STATUS status = MXL_TRUE;

  MxL_DLL_DEBUG0("MxLWare_API_GetDeviceStatus - %d\n", CmdType);

  switch (CmdType)
  {
    case MXL_DEV_ID_VERSION_REQ:
      status = MxL_GetDeviceInfo((PMXL_DEV_INFO_T)ParamPtr);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare_API_ConfigDemod 
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : The demod block specific configuration shall be handled 
--|                 through this API
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxLWare_API_ConfigDemod(MXL_CMD_TYPE_E CmdType, void *ParamPtr)
{
  MXL_STATUS status = MXL_TRUE;

  switch (CmdType)
  {
    case MXL_DEMOD_SYMBOL_RATE_CFG:
      status = MxL_ConfigDemodSymbolRate((PMXL_SYMBOL_RATE_T)ParamPtr);
      break;

    case MXL_DEMOD_MPEG_OUT_CFG:
      status = MxL_ConfigDemodMpegOutIface((PMXL_MPEG_OUT_CFG_T)ParamPtr);
      break;

    case MXL_DEMOD_ANNEX_QAM_TYPE_CFG:
      status = MxL_ConfigDemodAnnexQamType((PMXL_ANNEX_CFG_T)ParamPtr);
      break;

    case MXL_DEMOD_MISC_SETTINGS_CFG:
      break;

    case MXL_DEMOD_INTR_MASK_CFG:
      status = MxL_ConfigDemodInterrupt((PMXL_INTR_CFG_T)ParamPtr);
      break;

    case MXL_DEMOD_INTR_CLEAR_CFG:
      status = MxL_ClearDemodInterrupt((PMXL_INTR_CLEAR_T)ParamPtr);
      break;

    case MXL_DEMOD_RESET_STAT_COUNTER_CFG:
      status = MxL_ResetDemodStatCounters((PMXL_RESET_COUNTER_T)ParamPtr);
      break;

    case MXL_DEMOD_ADC_IQ_FLIP_CFG:
      status = MxL_ConfigDemodAdcIqFlip((PMXL_ADCIQFLIP_CFG_T)ParamPtr);
      break;

    case MXL_DEMOD_QAM_BURST_FREEZE_CFG:
      status = MxL_ConfigDemodQamBurstFreeze((PMXL_QAM_BURST_FREEZE_T)ParamPtr);
      break;

    case MXL_DEMOD_INVERT_CARRIER_OFFSET_CFG:
      status = MxL_ConfigDemodInvertCarrierOffset((PMXL_INVERT_CARR_OFFSET_T)ParamPtr);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare_API_GetDemodStatus 
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : The demod specific inquiries shall be handled 
--|                 through this API
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxLWare_API_GetDemodStatus(MXL_CMD_TYPE_E CmdType, void *ParamPtr)
{
  MXL_STATUS status = MXL_TRUE;

  switch (CmdType)
  {
    case MXL_DEMOD_INTR_STATUS_REQ:
      status = MxL_GetDemodInterruptStatus((PMXL_INTR_STATUS_T)ParamPtr);
      break;

    case MXL_DEMOD_FEC_LOCK_REQ:
    case MXL_DEMOD_MPEG_LOCK_REQ:
    case MXL_DEMOD_QAM_LOCK_REQ:
      status = MxL_GetDemodLockStatus(CmdType,  (PMXL_DEMOD_LOCK_STATUS_T)ParamPtr);
      break;

    case MXL_DEMOD_SNR_REQ:
      status = MxL_GetDemodSnr((PMXL_DEMOD_SNR_INFO_T)ParamPtr);
      break;

    case MXL_DEMOD_BER_UNCODED_BER_CER_REQ:
      status = MxL_GetDemodBer((PMXL_DEMOD_BER_INFO_T)ParamPtr);
      break;

    case MXL_DEMOD_ANNEX_QAM_TYPE_REQ:
      status = MxL_GetDemodAnnexQamType((PMXL_DEMOD_ANNEXQAM_INFO_T)ParamPtr);
      break;

    case MXL_DEMOD_CARRIER_OFFSET_REQ:
      status = MxL_GetDemodCarrierOffset((PMXL_DEMOD_CARRIEROFFSET_INFO_T)ParamPtr);
      break;

    case MXL_DEMOD_INTERLEAVER_DEPTH_REQ:
      status = MxL_GetDemodInterleaverDepth((PMXL_INTERDEPTH_INFO_T)ParamPtr);
      break;

    case MXL_DEMOD_EQUALIZER_FILTER_REQ:
      status = MxL_GetDemodEqualizerFilter((PMXL_DEMOD_EQUALFILTER_INFO_T)ParamPtr);
      break;
      
    case MXL_DEMOD_STAT_COUNTERS_REQ:
      status = MxL_GetDemodStatisticCounters((PMXL_DEMOD_STAT_COUNT_T)ParamPtr);
      break;
      
    case MXL_DEMOD_TIMING_OFFSET_REQ:
      status = MxL_GetDemodTimingOffset((PMXL_DEMOD_TIMINGOFFSET_INFO_T)ParamPtr);
      break;

    case MXL_DEMOD_RETUNE_INDICATOR_REQ:
      status = MxL_GetDemodReTuneIndicator((PMXL_RETUNE_IND_T)ParamPtr);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare_API_ConfigTuner 
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : The tuner block specific configuration shall be handled 
--|                 through this API
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxLWare_API_ConfigTuner(MXL_CMD_TYPE_E CmdType, void *ParamPtr)
{
  MXL_STATUS status = MXL_TRUE;

  MxL_DLL_DEBUG0("MxLWare_API_ConfigTuner : CmdType =%d\n", CmdType);
  
  switch (CmdType)
  {
    case MXL_TUNER_AGC_SETTINGS_CFG:
      status = MxL_ConfigTunerAgc((PMXL_AGC_T)ParamPtr);
      break;

    case MXL_TUNER_TOP_MASTER_CFG:
      status = MxL_ConfigTunerTopMaster((PMXL_TOP_MASTER_CFG_T)ParamPtr);
      break;

    case MXL_TUNER_CHAN_TUNE_CFG:
      status = MxL_ConfigTunerChanTune((PMXL_RF_TUNE_CFG_T)ParamPtr);
      break;

    case MXL_TUNER_CHAN_DEPENDENT_TUNE_CFG:
      status = MxL_ConfigTunerChanDependent((PMXL_CHAN_DEPENDENT_T)ParamPtr);
      break;

    case MXL_TUNER_AGC_LOCK_SPEED_CFG:
      status = MxL_ConfigTunerAgcLockSpeed((PMXL_AGC_CTRL_SPEED_T)ParamPtr);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare_API_GetTunerStatus 
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 6/19/2009
--|
--| DESCRIPTION   : The tuner specific inquiries shall be handled 
--|                 through this API
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS MxLWare_API_GetTunerStatus(MXL_CMD_TYPE_E CmdType, void *ParamPtr)
{
  MXL_STATUS status;

  MxL_DLL_DEBUG0("MxLWare_API_GetTunerStatus : CmdType =%d\n", CmdType);
  
  switch (CmdType)
  {
    case MXL_TUNER_LOCK_STATUS_REQ:
      status = MxL_GetTunerLockStatus((PMXL_TUNER_LOCK_STATUS_T)ParamPtr);
      break;

    case MXL_TUNER_RF_RX_PWR_REQ:
      status = MxL_GetTunerRxPower((PMXL_TUNER_RX_PWR_T)ParamPtr);
      break;

    case MXL_TUNER_AGC_SETTINGS_REQ:
      status = MxL_GetTunerAgcSettings((PMXL_AGC_T)ParamPtr);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}

