#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "logging.h"
#include "csm_all.h"
#include "utils.h"

// System includes ( @added )
#include <string>
// ROS includes ( @added )
#include "rclcpp/rclcpp.hpp"


int sm_debug_write_flag = 0;

const char * sm_program_name = 0;

void sm_debug_write(int flag) {
	sm_debug_write_flag = flag;
}

char sm_program_name_temp[256];
void sm_set_program_name(const char*name) {
	my_basename_no_suffix(name, sm_program_name_temp);
	sm_program_name = sm_program_name_temp;
}

int checked_for_xterm_color = 0;
int xterm_color_available = 0;


void check_for_xterm_color() {
	if(checked_for_xterm_color) return;
	checked_for_xterm_color = 1;
	
	const char * term = getenv("TERM");
	if(!term) term = "unavailable";
	xterm_color_available = !strcmp(term, "xterm-color") || !strcmp(term, "xterm") 
		|| !strcmp(term, "rxvt");
/*	sm_info("Terminal type: '%s', colors: %d\n", term, xterm_color_available); */
}

#define XTERM_COLOR_RED "\e[1;37;41m"
#define XTERM_COLOR_RESET "\e[0m"

#define XTERM_ERROR XTERM_COLOR_RED
#define XTERM_DEBUG "\e[1;35;40m"

/**
 * @modified Context returned by value instead of written to 'stdout' (old name: sm_write_context())
 */
std::string sm_get_context();

// Max length of the log message
constexpr std::size_t MAX_LOG = 512;
// Log message format buffer
char sm_log_buffer[MAX_LOG] = "";

void sm_error(const char *msg, ...)
{
    /**
     * @removed 
     *
     *  check_for_xterm_color();
     *  if(xterm_color_available)
     *      fprintf(stderr, XTERM_ERROR);
     *      
     *  if(sm_program_name) 
     *      fprintf(stderr, "%s: ", sm_program_name);
     *  
     *  sm_get_context();
     *  
     *  if(!xterm_color_available) 
     *      fputs(":err: ", stderr);
     */

    /**
     * @modifief vfprint(...) call modified to vsprintf(...) combined with the ROS2 logging API
     */

    // Parse va_list
	va_list ap;
	va_start(ap, msg);
	vsnprintf(sm_log_buffer, MAX_LOG, msg, ap);
    // Log error
	RCLCPP_ERROR_STREAM(rclcpp::get_logger("csm-lib"), sm_get_context() << sm_log_buffer);
    
}

void sm_info(const char *msg, ...)
{
    /**
     * @removed 
     * 
     *  check_for_xterm_color();
     *  
     *  if(sm_program_name) 
     *      fprintf(stderr, "%s: ", sm_program_name);
     *  
     *  sm_get_context();
     *  
     *  if(!xterm_color_available) 
     *      fputs(":inf: ", stderr);
     */
	
    /**
     * @modifief vfprint(...) call modified to vsprintf(...) combined with the ROS2 logging API
     */

    // Parse va_list
	va_list ap;
	va_start(ap, msg);
	vsnprintf(sm_log_buffer, MAX_LOG, msg, ap);
    // Log error
	RCLCPP_INFO_STREAM(rclcpp::get_logger("csm-lib"), sm_get_context() << sm_log_buffer);
    
}

void sm_debug(const char *msg, ...)
{
	if(!sm_debug_write_flag)
        return;

    /**
     * @removed 
     * 	
     *   check_for_xterm_color();
     *   
     *   if(xterm_color_available)
     *       fprintf(stderr, XTERM_DEBUG);
     *   
     *   if(sm_program_name) 
     *       fprintf(stderr, "%s: ", sm_program_name);
     *  
     *   sm_get_context();
     *  
     *   if(!xterm_color_available) 
     *       fputs(":dbg: ", stderr);
	 */

    /**
     * @modifief vfprint(...) call modified to vsprintf(...) combined with the ROS2 logging API
     */
    
    // Parse va_list
	va_list ap;
	va_start(ap, msg);
	vsnprintf(sm_log_buffer, MAX_LOG, msg, ap);
    // Log error
	RCLCPP_DEBUG_STREAM(rclcpp::get_logger("csm-lib"), sm_get_context() << sm_log_buffer);

}

#define MAX_CONTEXTS 1000
const char * sm_log_context_name[MAX_CONTEXTS] = {""};
int sm_log_context = 0;

void sm_log_push(const char*cname) {

    /**
     * @modified Changed sprintf(...) to snprintf(...), removed trailing newline character
     */
	if(sm_debug_write_flag) {
        constexpr std::size_t MAX_MESSAGE_LEN = 1024;
		char message[MAX_MESSAGE_LEN];
        snprintf(message, MAX_MESSAGE_LEN, "  ___ %s", cname);
		sm_debug(message);
	}

	assert(sm_log_context<MAX_CONTEXTS-1);
	sm_log_context++;
	sm_log_context_name[sm_log_context] = cname;
}


void sm_log_pop() {
	assert(sm_log_context>0);
	sm_log_context--;
}

/**
 * @modified Function modified to return contextual intendation instead of writting it
 *    to the 'stdout'
 */
std::string sm_get_context() {
    std::string ret;
	for(int i=0;i<sm_log_context;i++)
		ret.append("   ");
	return ret;
}

