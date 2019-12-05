#ifndef MAP_OF_MEM_H
#define MAP_OF_MEM_H

/* Address of GDT/PDG in memory */
#define ADDR_PGD                        0x350000
#define ADDR_PTB_0                      ADDR_PGD + PAGE_SIZE
#define ADDR_PTB_USER                   ADDR_PGD + 2*PAGE_SIZE

/* Address of task user in memory */
#define ADDR_TASK_USER1                 0x601000                      /*   TASK 1   */
#define ADDR_TASK_USER1_DATA            ADDR_TASK_USER1               /* |  DATA  | <-- 0x601000 */
#define ADDR_TASK_USER1_CODE            ADDR_TASK_USER1 + PAGE_SIZE   /* |  CODE  | <-- 0x602000 */
#define ADDR_TASK_USER1_STACK_USER      ADDR_TASK_USER1 + (2*PAGE_SIZE) /* | USTACK | <-- 0x603000 */
#define ADDR_TASK_USER1_STACK_KERNEL    ADDR_TASK_USER1 + (3*PAGE_SIZE) /* | KSTACK | <-- 0x604000 */

#define ADDR_PGD_USER1                  0x605000
#define ADDR_PTB_KRN_USER1              ADDR_PGD_USER1 + PAGE_SIZE
#define ADDR_PTB_USER1                  ADDR_PGD_USER1 + 2*PAGE_SIZE

/* Address of task user in memory */
#define ADDR_TASK_USER2                 0x611000                      /*   TASK 2   */
#define ADDR_TASK_USER2_DATA            ADDR_TASK_USER1_DATA          /* |  DATA  | <-- 0x601000 */
#define ADDR_TASK_USER2_CODE            ADDR_TASK_USER2 + PAGE_SIZE   /* |  CODE  | <-- 0x612000 */
#define ADDR_TASK_USER2_STACK_USER      ADDR_TASK_USER2 + 2*PAGE_SIZE /* | USTACK | <-- 0x613000 */
#define ADDR_TASK_USER2_STACK_KERNEL    ADDR_TASK_USER2 + 3*PAGE_SIZE /* | KSTACK | <-- 0x614000 */

#define ADDR_PGD_USER2                 0x615000
#define ADDR_PTB_KRN_USER2             ADDR_PGD_USER2+ PAGE_SIZE
#define ADDR_PTB_USER2                 ADDR_PGD_USER2+ 2*PAGE_SIZE


#endif