/**
* @file    hello.c
* @author  Derek Molloy
* @date    4 April 2015
* @version 0.1
* @brief  An introductory "Hello World!" loadable kernel module (LKM) that can display a message
* in the /var/log/kern.log file when the module is loaded and removed. The module can accept an
* argument when it is loaded -- the name, which appears in the kernel log files.
* @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/
 
#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
 
int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);
 
MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Helene og Natalie");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Lala");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module
 
struct proc_dir_entry *procPtr = NULL;        ///< An example LKM argument -- default value is "world"
//module_param(procPtr, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
//MODULE_PARM_DESC(procPtr, "The name to display in /var/log/kern.log");  ///< parameter description
 
/** @brief The LKM initialization function
*  The static keyword restricts the visibility of the function to within this C file. The __init
*  macro means that for a built-in driver (not a LKM) the function is only used at initialization
*  time and that it can be discarded and its memory freed up after that point.
*  @return returns 0 if successful
*/
static int __init myProc_init(void){
   procPtr = create_proc_entry("myproc", 0644, NULL);
   procPtr->read_proc = procfile_read;
   return 0;
}
 
/** @brief The LKM cleanup function
*  Similar to the initialization function, it is static. The __exit macro notifies that if this
*  code is used for a built-in driver (not a LKM) that this function is not required.
*/
static void __exit myProc_exit(void){
   remove_proc_entry("myproc", NULL);
}

int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	if (offset > 0)
	{
		return 0;
	}
	else
	{
		return sprintf(buffer, "Hello world\n");
	}
}
 
/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
*  identify the initialization function at insertion time and the cleanup function (as
*  listed above)
*/
module_init(myProc_init);
module_exit(myProc_exit);
