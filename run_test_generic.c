#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BILLION  1000000000L;

#define REPEAT 20

#ifndef SUITE
#error SUITE undefined
#endif

int errno;

int main( int argc, char** argv )
{
  struct timespec start, stop;
  double accum;
  int i,j;



  int tests_n = sizeof(tests) / sizeof(char *);

  // filename dance
  time_t now = time(NULL);
  char * date_time = ctime(&now);

  i = 0;
  while(date_time[i] != '\0') {

    if (date_time[i] == ' ')
      date_time[i] = '_';
    else if (date_time[i] == '\n') {
      date_time[i] = '\0';
      break;
    }

    i++;
  }

  char filename_buf[64]; // 64 = arbitrary, sounds reasonable
  char command_buf[128]; // ditto
  char echo_buf[128]; // ditto
  char hostname[128]; // ditto

  if ( (errno == gethostname(hostname, sizeof(hostname))) != 0 ) {
    perror("Cannot retrieve hostname!\n");
    return EXIT_FAILURE;
  }

  filename_buf[0] = '\0';
  sprintf(filename_buf, "benchmark.txt");

  // rock on!
  FILE * results = fopen(filename_buf, "w");

  if (results == NULL) {
    perror("Cannot open the output file\n");
    return EXIT_FAILURE;
  }

  //  fprintf(results, "host, suite, name, run, time\n");

  for(j = 0; j < tests_n; j++) {

    for(i = 0; i < REPEAT; i++) {

      printf("%s@%s [%s]: %d/%d repeats - %d/%d tests\n", 
	     SUITE, hostname, names[j], i + 1, REPEAT, j + 1, tests_n);

      if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
	perror( "clock gettime" );
	return EXIT_FAILURE;
      }

      errno = system(tests[j]);

      if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
	perror( "clock gettime" );
	return EXIT_FAILURE;
      }

      accum = ( stop.tv_sec - start.tv_sec )
	+ (double)( stop.tv_nsec - start.tv_nsec )
	/ (double)BILLION;

      /* only output data, if the test was successful */
      if ( errno == 0 ) 
	fprintf(results, "%s,%s,%s,%d,%lf\n", hostname, SUITE, names[j], i, accum );
    }
  }

    fclose(results); 


  return EXIT_SUCCESS;
}
