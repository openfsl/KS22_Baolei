/**************************************************************************************************
  Filename:       OnBoard.h
  Revised:        $Date: 2012-03-29 12:09:02 -0700 (Thu, 29 Mar 2012) $
  Revision:       $Revision: 29943 $
**************************************************************************************************/

#ifndef ONBOARD_H
#define ONBOARD_H

#ifdef __cplusplus
extern "C"
{
#endif

#define HAL_ENABLE_INTERRUPTS()         //st( EA = 1; )
#define HAL_DISABLE_INTERRUPTS()        //st( EA = 0; )
#define HAL_INTERRUPTS_ARE_ENABLED()    //(EA)

typedef unsigned char halIntState_t;
#define HAL_ENTER_CRITICAL_SECTION(x)   //st( x = EA;  HAL_DISABLE_INTERRUPTS(); )
#define HAL_EXIT_CRITICAL_SECTION(x)    //st( EA = x; )
#define HAL_CRITICAL_STATEMENT(x)       //st( halIntState_t _s; HAL_ENTER_CRITICAL_SECTION(_s); x; HAL_EXIT_CRITICAL_SECTION(_s); )

#define __no_init	
#define MAXMEMHEAP  3230
#ifdef HALNODEBUG
#define HAL_ASSERT(expr)
#define HAL_ASSERT_FORCED()
#define HAL_ASSERT_STATEMENT(statement)
#define HAL_ASSERT_DECLARATION(declaration)
#else
#define HAL_ASSERT(expr)                        st( if (!( expr )) halAssertHandler(); )
#define HAL_ASSERT_FORCED()                     halAssertHandler()
#define HAL_ASSERT_STATEMENT(statement)         st( statement )
#define HAL_ASSERT_DECLARATION(declaration)     declaration
#endif

#ifdef __cplusplus
}
#endif

#endif // ONBOARD_H
