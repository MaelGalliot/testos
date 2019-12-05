#include <task.h>
#include <string.h>
#include <debug.h>
#include <segmem.h>
#include <pagemem.h>

/*
 * Init a user task in memory  
 * ------------------------
 * Example in memory : 
 *   TASK 1   
 * |  DATA  | <-- 0x806000 
 * |  CODE  | <-- 0x807000 
 * | USTACK | <-- 0x808000 
 * | KSTACK | <-- 0x809000 
 */
void init_user_task(int task_number, task_t * task, void * user_code, uint32_t addr_data, uint32_t addr_code, uint32_t addr_stack_user, uint32_t addr_kernel_stack){
  debug("[%s] to [%p - %p] \n",__func__,addr_data,addr_kernel_stack);
  init_pgd_task(task_number);
  memcpy((char *) addr_code, &user_code, PAGE_SIZE);    /* Copy the user code to section code of task */

  task->addr_task_data  = addr_data;
  task->addr_task_code  = addr_code;
  task->user_stack      = addr_stack_user + PAGE_SIZE - 1;  /* Stack starts at the end */
  task->kernel_stack    = addr_kernel_stack + PAGE_SIZE - 1;/* Stack starts at the end */
  task->cs_task         = c3_sel;
  task->ss_task         = d3_sel;
  task->flags_task      = get_flags();
  if(task_number==1)
    task->cr3 = (pde32_t *) ADDR_PGD_USER1;
  else
    task->cr3 = (pde32_t *) ADDR_PGD_USER2;

  
}

/*
 * Update ebp of the task currently execute 
 */
void tss_change_s0_esp(uint32_t esp_of_current_task){
  tss_t tss; get_tr(tss);
  tss.s0.esp = esp_of_current_task; /* Update ebp */
}

void init_pgd_task(int user){
  int i;
  pde32_t * pgd;
  pde32_t * ptb0;
  pde32_t * ptb_user;
  if(user==1){
    pgd = (pde32_t *) ADDR_PGD_USER1;             /* Create pointer of PGD */
    ptb0 = (pde32_t *)ADDR_PTB_KRN_USER1;           /* Create first kernel PTB_0  [0 - 0x400000] */
    ptb_user = (pde32_t *)ADDR_PTB_USER1;           /* Create second kernel PTB_1 [0x400000 - 0x800000] */
  } else {
    pgd = (pde32_t *) ADDR_PGD_USER2;             /* Create pointer of PGD */
    ptb0 = (pde32_t *)ADDR_PTB_KRN_USER2;           /* Create first kernel PTB_0  [0 - 0x400000] */
    ptb_user = (pde32_t *)ADDR_PTB_USER2;           /* Create second kernel PTB_1 [0x400000 - 0x800000] */
  }
  
  memset((void *) pgd, 0,PAGE_SIZE);      //Clean PGD 
  memset((void *) ptb0, 0,PAGE_SIZE);     //Clean PTB_0 
  memset((void *) ptb_user, 0,PAGE_SIZE);     //Clean PTB_0 

  /* A savoir lors de l'activation de la pagination les adresses mappées sont additionées multiplié par 0x1000 sur PAGE_SIZE */

  //Mappinge of PTB_0 [0x000000 - 0x400000]
  for(i=0;i<1024;i++){  
    pg_set_entry(&ptb0[i],PG_KRN|PG_RW,i);
  }

  pg_set_entry(&pgd[0],PG_KRN|PG_RW, page_nr(&ptb0[0]));      /* PDE - [0x000000 - 0x400000] */
  pg_set_entry(&pgd[1],PG_USR|PG_RW, page_nr(&ptb_user[0]));      /* PDE - [0x000000 - 0x400000] */

    pte32_t * pte_task_data_user; 
    pte32_t * pte_task_code_user; 
    pte32_t * pte_task_stack_user; 
    pte32_t * pte_task_stack_kernel_user; 

    pte32_t * pte_task_pgd; 
    pte32_t * pte_task_ptb_krn; 
    pte32_t * pte_task_ptb_user; 
  if(user==1){
    pte_task_data_user = (pte32_t *)ADDR_TASK_USER1_DATA;
    pte_task_code_user = (pte32_t *)ADDR_TASK_USER1_CODE;
    pte_task_stack_user = (pte32_t *)ADDR_TASK_USER1_STACK_USER;
    pte_task_stack_kernel_user = (pte32_t *)ADDR_TASK_USER1_STACK_KERNEL;

    pte_task_pgd = (pte32_t *)ADDR_PGD_USER1;
    pte_task_ptb_krn = (pte32_t *)ADDR_PTB_KRN_USER1;
    pte_task_ptb_user = (pte32_t *)ADDR_PTB_USER1;
  } else {
    pte_task_data_user = (pte32_t *)ADDR_TASK_USER2_DATA;
    pte_task_code_user = (pte32_t *)ADDR_TASK_USER2_CODE;
    pte_task_stack_user = (pte32_t *)ADDR_TASK_USER2_STACK_USER;
    pte_task_stack_kernel_user = (pte32_t *)ADDR_TASK_USER2_STACK_KERNEL;

    pte_task_pgd = (pte32_t *)ADDR_PGD_USER2;
    pte_task_ptb_krn = (pte32_t *)ADDR_PTB_KRN_USER2;
    pte_task_ptb_user = (pte32_t *)ADDR_PTB_USER2;
  }

  if(user==1){
    //Mapping of share data user1/user2
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_USR | PG_RW, pg_4K_nr((int)pte_task_data_user));//0x601000
    
    //Mapping of code user1
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_USR | PG_RO, pg_4K_nr((int)pte_task_code_user));//0x602000

    //Mapping of stack user1 
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_USR | PG_RW, pg_4K_nr((int)pte_task_stack_user));//0x603000

    //Mapping of stack kernel user1
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_KRN | PG_RW, pg_4K_nr((int)pte_task_stack_kernel_user));//0x604000

    //Mapping PGD
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_KRN | PG_RW, pg_4K_nr((int)pte_task_pgd));//0x605000
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_KRN | PG_RW, pg_4K_nr((int)pte_task_ptb_krn));//0x606000
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_KRN | PG_RW, pg_4K_nr((int)pte_task_ptb_user));//0x607000
    debug("PGD [%s] to [%p - %p] \n",__func__,ADDR_PGD_USER1,ADDR_PTB_USER1);
  } else {
    //Mapping of share data user1/user2
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_USR | PG_RW, pg_4K_nr((int)pte_task_data_user));//0x601000
    
    //Mapping of code user1
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_USR | PG_RO, pg_4K_nr((int)pte_task_code_user));//0x612000

    //Mapping of stack user1 
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_USR | PG_RW, pg_4K_nr((int)pte_task_stack_user));//0x613000

    //Mapping of stack kernel user1
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_KRN | PG_RW, pg_4K_nr((int)pte_task_stack_kernel_user));//0x614000

    //Mapping PGD
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_KRN | PG_RW, pg_4K_nr((int)pte_task_pgd));//0x615000
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_KRN | PG_RW, pg_4K_nr((int)pte_task_ptb_krn));//0x616000
    pg_set_entry(&ptb_user[pt32_idx(pte_task_data_user)], PG_KRN | PG_RW, pg_4K_nr((int)pte_task_ptb_user));//0x617000
    debug("PGD [%s] to [%p - %p] \n",__func__,ADDR_PGD_USER2,ADDR_PTB_USER2);
  }
}


/*
 * First user task who counts
 */
void user1(){
  //int * counter=(int *)ADDR_TASK_USER1_DATA;
  while(1){
    //*counter+=1;
    //asm volatile("int $48"::"S"("test\n")); 
  };
} 

/*
 * Second user task who display counter
 */
void user2(){
  while(1){
    //asm volatile("int $48"::"S"("test\n")); 
  };
} 