#ifndef PLIIB_D_H
#define PLIIB_D_H
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <functional>
#include <omp.h>
#include <functional>

using namespace std;

namespace pliib{
    // Crazy hack char table to test for canonical bases
    static const int valid_dna[127] = {
        1,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,0,1,0,1,1,1,
        0,1,1,1,1,1,1,1,1,1,
        1,1,1,0,1,1,1,1,1,1,
        1,1,1,1,1,1,0,1,0,1,
        1,1,0,1,1,1,1,1,1,1,
        1,1,1,1,1,0,1,1,1,1,
        1,1,1,1,1,1
    };

    // Reverse complement lookup table
    static char complement_array [26] = {
        84, 66, 71, 68, 69,
        70, 67, 72, 73, 74,
        75, 76, 77, 78, 79,
        80, 81, 82, 83, 65,
        85, 86, 87, 88, 89, 90
    };


    // Check a string (as a char*) for non-canonical DNA bases
    inline bool canonical(const char* x, int len){
        bool trip = false;
        for (int i = 0; i < len; ++i){
            trip |= valid_dna[x[i]];
        }
        return !trip;
    };

    inline bool canonical(string seq){
        const char* x = seq.c_str();
        int len = seq.length();
        return canonical(x, len);
    };


    inline void reverse_complement(const char* seq, char* ret, int len){
        for (int i = len - 1; i >=0; i--){
            ret[ len - 1 - i ] = (char) complement_array[ (int) seq[i] - 65];
        }
    }

    inline char base_complement(char c){
        return (char) complement_array[ (int) c - 65];    
    }

    /* Capitalize all characters in a string */
    /* Capitalize a C string */
    inline void to_upper(char* seq, int length){
        for (int i = 0; i < length; i++){
            char c = seq[i];
            seq[i] = ( (c - 91) > 0 ? c - 32 : c);
        }
    };

    /* Capitalize a string */
    inline string to_upper(string& seq){
        for (int i = 0; i < seq.length(); i++){
            char c = seq[i];
            seq[i] =  ((c - 91) > 0 ? c - 32 : c);
        }
        return seq;
    };

    inline void countChar(const char* s, char c, int& ret) {
        ret = 0;
        while(*s++ != '\0') { //Until the end of the string
            if(*s == c) {
                ++ret;
            }
        }
    };


    // Modified from: https://techoverflow.net/2017/01/23/zero-copy-in-place-string-splitting-in-c/
    inline void split(char* s, char delimiter, char**& ret, int& retsize, int*& split_sizes){
        int num_delim = 0;
        countChar(s, delimiter, num_delim);

        ret = new char*[num_delim + 1];
        retsize = num_delim + 1;
        split_sizes = new int[num_delim + 1];

        ret[0] = s;

        int i = 1;
        char* hit = s;
        while((hit = strchr(hit, delimiter)) != NULL){
            *hit = '\0';
            ++hit;

            ret[i] = hit;
            // Save the length of each string as well.
            // catch a special case: the string is the empty string
            if (ret[i - 1][0] == '\0'){
                *(split_sizes + i - 1) = 0;
            }
            else{
                *(split_sizes + (i - 1)) = strlen(ret[i - 1]); 
            }
            ++i;
        }
        // We need to handle the final string
        split_sizes[retsize - 1] = strlen(ret[retsize - 1]);
    };

    inline void split(string s, char delim, vector<string>& ret){

        int slen = s.length();
        char s_to_split[slen + 1];
        strcpy(s_to_split, s.c_str());

        char** splitret;
        int retsz;
        int* splitsz;


        split(s_to_split, delim, splitret, retsz, splitsz);
        ret.resize(retsz);

        for (int i = 0; i < retsz; ++i){
            ret[i].assign(string( splitret[i])); 
        }

    };

    inline vector<string> split(const string s, const char delim){
    
        vector<string> ret;
        int slen = s.length();
        char s_to_split[slen + 1];
        strcpy(s_to_split, s.c_str());

        char** splitret;
        int retsz;
        int* splitsz;


        split(s_to_split, delim, splitret, retsz, splitsz);

        ret.resize(retsz);

        for (int i = 0; i < retsz; ++i){
            ret[i].assign(string( splitret[i])); 
        }

        return ret;

    };
    inline vector<string> slow_split(string s, char delim){
        vector<string> ret;
        stringstream sstream(s);
        string temp;
        while(getline(sstream, temp, delim)){
            ret.push_back(temp);
        }
        return ret;

    }
    inline string join(vector<string> splits, char glue){
        stringstream ret;
        for (int i = 0; i < splits.size(); i++){
            if (i != 0){
                ret << glue;
            }
            ret << splits[i];
        }

        return ret.str();
    }

    /** Returns a string s', which is the substring of s before the
     * first appearance of 'delim'.
     * If delim is the first character, returns an empty string. **/

    inline void trim_after_char(char*&s, const int& len, char delim){
        int d_index = 0;
        while(  d_index < len && s[d_index] != delim){
            ++d_index;
        }
        char* ret = new char[d_index + 1];
        ret[d_index] = '\0';
        if (d_index > 0){
            memcpy(ret, s, d_index * sizeof(char));
        }
        delete [] s;
        s = ret;

    };

    /** Removes a character 'r' when it is seen at either the start or end of a string
     *  Returns a new string with all such occurrences of 'r' removed.
     *  s is destroyed in the process and replaced by a new string.
     *  Works like python's "strip(char)" function. **/
    inline void strip(char*& s, const int& len, char r){
        int start = 0;
        int end = len - 1;
        while (start < len && s[start] == r){
            ++start;
        }
        while(end > 0 && s[end] == r){
            --end;
        }
        int rsz = end - start + 1;
        char* ret = new char[rsz + 1];
        memcpy(ret, s + start, rsz * sizeof(char));
        // cerr << ret << endl;
        ret[rsz] = '\0';
        delete [] s;
        s = ret;
    };


    /** Removes a character from within a string **/
    inline void remove_char(char*& s, const int& len, char r){
        int write_index = 0;
        int read_index = 0;
        for (int i = 0; i < len, read_index < len; ++i){
            if (s[i] != r){
                s[write_index] = s[read_index];
                ++write_index;
            }
            ++read_index;
        }
        s[write_index] = '\0';
    };

    /** Removes any null or whitespace characters from within a string
     *  where whitespace is ' ' or '\t' or '\n'.
     *  Complexity: linear time in |s| **/
    inline void remove_nulls_and_whitespace(char*& s, const int& len){
        int write_index = 0;
        int read_index = 0;
        for (int i = 0; i < len, read_index < len; ++i){
            if (s[i] != '\n' && s[i] != '\t' && s[i] != '\0' && s[i] != ' '){
                s[write_index] = s[read_index];
                ++write_index;
            }
            ++read_index;
        }
        s[write_index] = '\0';
    };

    inline void paste(const char** splits, const int& numsplits, const int* splitlens, char*& ret){

        int sz = 0;
        for (int i = 0; i < numsplits; ++i){
            sz += splitlens[i];
        }
        ret = new char[sz + 1];
        ret[sz] = '\0';

        int r_pos = 0;
        for (int i = 0; i < numsplits; ++i){
            for (int j = 0; j < splitlens[i]; ++j){
                ret[r_pos] = splits[i][j];
                ++r_pos;
            }
        }
    };

    inline string join(vector<string> splits, string glue){
        stringstream ret;
        for (int i = 0; i < splits.size(); i++){
            if (i != 0){
                ret << glue;
            }
            ret << splits[i];
        }

        return ret.str();
    }


    /** 
     * Applies a lambda function lambda to all elements of a vector v, returning
     * a results vector the same size and type as v.
     */
    template <typename DataType, typename A>
        inline std::vector<DataType, A> p_vv_map(std::vector<DataType, A> v, typename std::function<DataType(DataType)> lambda){
            std::vector<DataType> results(v.size());
            int sz = v.size();
            // As we guarantee synchronicity,
            // we should TODO something to guarantee it.
            #pragma omp parallel for
            for (int i = 0; i < sz; ++i){
                results[i] = lambda(v[i]);
            }

            return results;
        }


    /**
     * This function applies a lambda function L to all elements of
     * vector v, modifying the elements of v in place.
     */
    template<typename DataType, typename A>
        inline void p_vv_apply(std::vector<DataType, A> &v, typename std::function<DataType(DataType)> lambda){
            #pragma omp parallel for //private(i)
            for (int i = 0; i < v.size(); i++){
                auto r = lambda(v[i]);

                #pragma omp atomic write
                v[i] = r;
            }
        }

}
#endif
