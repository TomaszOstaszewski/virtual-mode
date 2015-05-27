/**
 * @file   paging.h
 * @author tomek <tomek@gtx-0.lan>
 * @date   Wed May 27 22:05:34 2015
 * 
 * @brief  
 * 
 * 
 */

#ifndef PAGING_H
#define PAGING_H

/** 
 * 
 * 
 * 
 * @return 
 */
uint32_t* get_pde_table(void);

/** 
 * 
 * 
 */
void identity_map_first_4Mb(void);

/** 
 * 
 * 
 */
void turn_on_paging(void);

#endif /* PAGING_H */

