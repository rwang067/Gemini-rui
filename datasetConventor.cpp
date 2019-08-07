#include <iostream>
#include<fstream>
#include<string>
#include <string.h>
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

typedef uint32_t vid_t;

#define CHUNKSIZE (1<<20)

int main(int argc, char const *argv[])
{
    std::string filename = "../dataset/LiveJournal/soc-LiveJournal1.txt";
    FILE * inf = fopen(filename.c_str(), "r");

    std::string outfilename = filename + ".binedgelist";
    int outf = open(outfilename.c_str(), O_WRONLY | O_CREAT, S_IROTH | S_IWOTH | S_IWUSR | S_IRUSR);
    
    char* buf = (char*)malloc(CHUNKSIZE*sizeof(vid_t)*2);
    char* bufptr = buf;

    char s[1024];
    int e = 0;
    while(fgets(s, 1024, inf) != NULL) {
        if (s[0] == '#') continue; // Comment
        if (s[0] == '%') continue; // Comment
        
        // logstream(LOG_INFO) << " s= " << s << std::endl;
        char *t1, *t2;
        t1 = strtok(s, "\t, ");
        t2 = strtok(NULL, "\t, ");
        if (t1 == NULL || t2 == NULL ) {
            std::cout << "Input file is not in right format. "
            << "Expecting <from> <to>. "
            << "Current line: " << s << "\n";
            assert(false);
        }
        vid_t from = atoi(t1);
        vid_t to = atoi(t2);
        if( from == to ) continue;
        *((vid_t*)bufptr) = from;
        bufptr += sizeof(vid_t);
        *((vid_t*)bufptr) = to;
        bufptr += sizeof(vid_t);
        e++;
        if( e == CHUNKSIZE ){
            write( outf, buf, bufptr - buf );
            bufptr = buf;
            e = 0;
        }
    }
    fclose(inf);
    write( outf, buf, bufptr - buf );
    close(outf);
    std::cout << "Totally " << e << " edges." << std::endl;
    return 0;
}
