#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

#define procfs_name "csinfo"

/**
 * This structure hold information about the /proc file
 *
 */
static
struct proc_dir_entry *Our_Proc_File;

static int
procfile_read(char *buffer,
	      char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
  int ret;
	
  /* 
   * We give all of our information in one go, so if the
   * user asks us if we have more information the
   * answer should always be no.
   *
   * This is important because the standard read
   * function from the library would continue to issue
   * the read system call until the kernel replies
   * that it has no more information, or until its
   * buffer is filled.
   */
  if (offset > 0) {
    /* we have finished to read, return 0 */
    ret  = 0;
  } else {
    /* fill the buffer, return the buffer size */
    ret = sprintf(buffer, "buildid: %s\nversion: %s\n",
		  STRINGIFY(BS_BUILDID), 
		  STRINGIFY(BS_BUILDTAG) );
  }

  return ret;
}

static
int __init my_init_module( void )
{
  Our_Proc_File = create_proc_entry(procfs_name, 0644, NULL);
	
  if (Our_Proc_File == NULL) {
    remove_proc_entry(procfs_name, NULL);
    printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
	   procfs_name);
    return -ENOMEM;
  }

  Our_Proc_File->read_proc = procfile_read;
  Our_Proc_File->mode 	 = S_IFREG | S_IRUGO;
  Our_Proc_File->uid 	 = 0;
  Our_Proc_File->gid 	 = 0;
  
  printk(KERN_INFO "/proc/%s created\n", procfs_name);	
  return 0;	/* everything is ok */
}

static
void __exit my_cleanup_module( void )
{
  remove_proc_entry(procfs_name, NULL);
  printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}

module_init( my_init_module );
module_exit( my_cleanup_module );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Obtain Cortina build id and version");
MODULE_AUTHOR("Andrew Peebles @cortina-systems.com");
