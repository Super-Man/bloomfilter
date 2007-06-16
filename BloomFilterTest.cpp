/*
 **************************************************************************
 *                                                                        *
 *                           Open Bloom Filter                            *
 *                                                                        *
 * Author: Arash Partow - 2000                                            *
 * URL: http://www.partow.net                                             *
 * URL: http://www.partow.net/programming/hashfunctions/index.html        *
 *                                                                        *
 * Copyright notice:                                                      *
 * Free use of the Bloom Filter Library is permitted under the guidelines *
 * and in accordance with the most current version of the Common Public   *
 * License.                                                               *
 * http://www.opensource.org/licenses/cpl.php                             *
 *                                                                        *
 **************************************************************************
*/

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <set>
#include <string>

#include "bloom_filter.hpp"


void read_file(const std::string file_name, std::vector<std::string>& buffer);
std::string reverse(std::string str);

void generate_outliers(const std::vector<std::string>& word_list, std::vector<std::string>& outliers);
void purify_outliers(const std::vector<std::string>& word_list,std::vector<std::string>& outliers);

int main(int argc, char* argv[])
{

   std::vector<std::string> word_list;
   std::vector<std::string> outliers;

   read_file("word-list-large-2.txt",word_list);

   if (word_list.empty())
   {
      std::cout << "ERROR: Input file invalid!" << std::endl;
      return 1;
   }

   generate_outliers(word_list,outliers);
   purify_outliers(word_list,outliers);

   unsigned int random_seed = 0;
   double total_false_positive = 0.0;

   while(random_seed < 1000)
   {
      double probability_of_false_positive = 1.0/(1.0 * word_list.size());
      bloom_filter  filter(word_list.size(),probability_of_false_positive,random_seed++);

      for(std::vector<std::string>::iterator it = word_list.begin(); it != word_list.end(); ++it)
      {
         filter.insert(*it);
      }

      for(std::vector<std::string>::iterator it = word_list.begin(); it != word_list.end(); ++it)
      {
         if (!filter.contains(*it))
         {
            std::cout << "ERROR: key not found! =>" << (*it) << std::endl;
         }
      }

      for(std::vector<std::string>::iterator it = outliers.begin(); it != outliers.end(); ++it)
      {
         if (filter.contains(*it))
         {
            filter.contains(*it);
            //std::cout << "ERROR: key that does not exist found! => " << (*it) << std::endl;
            total_false_positive++;
         }
      }

      std::size_t total_numnber_of_quries = (random_seed + 1) * (outliers.size() + word_list.size());
      double current_pfp = total_false_positive / (1.0 * total_numnber_of_quries);

      std::cout << "Round: " << random_seed <<
                   "\tTotal queries: " << total_numnber_of_quries  <<
                   "\tFalse queries: " << total_false_positive <<
                   "\tIPFP:"           << probability_of_false_positive <<
                   "\tPFP:"            << current_pfp <<
                   "\tDPFP:"           << current_pfp - probability_of_false_positive << std::endl;

   }

   return true;
}


void read_file(const std::string file_name, std::vector<std::string>& buffer)
{
   /* Assumes no whitespace in the lines being read in. */
   std::ifstream in_file(file_name.c_str());
   if (!in_file)
   {
      return;
   }

   std::istream_iterator< std::string > is(in_file);
   std::istream_iterator< std::string > eof;
   std::copy( is, eof, std::back_inserter(buffer));
}

std::string reverse(std::string str)
{
   char tempch;

   /* Reverse the string */
   for(unsigned int i = 0; i < (str.length() / 2); i++)
   {
      tempch = str[i];
      str[i] = str[str.length() - i - 1];
      str[str.length() - i - 1] = tempch;
   }

   return str;
}

void generate_outliers(const std::vector<std::string>& word_list, std::vector<std::string>& outliers)
{
   for(std::vector<std::string>::const_iterator it = word_list.begin(); it != word_list.end(); ++it)
   {
      if ((*it) != reverse((*it)))
      {
         outliers.push_back((*it) + reverse((*it)));
         outliers.push_back((*it) + (*it));
         outliers.push_back(reverse((*it)) + (*it) + reverse((*it)));
      }
   }
   std::sort(outliers.begin(),outliers.end());
}

void purify_outliers(const std::vector<std::string>& word_list,std::vector<std::string>& outliers)
{
   std::set<std::string> set1;
   std::set<std::string> set2;

   std::copy(word_list.begin(),word_list.end(),std::inserter(set1,set1.begin()));
   std::copy(outliers.begin(),outliers.end(),std::inserter(set2,set2.begin()));

   std::vector<std::string> intersect_list;

   std::set_intersection(set1.begin(),set1.end(),
                         set2.begin(),set2.end(),
                         std::back_inserter(intersect_list));

   if(!intersect_list.empty())
   {
      for(std::vector<std::string>::iterator it = intersect_list.begin(); it != intersect_list.end(); ++it)
      {
         std::vector<std::string>::iterator dup_it = std::find(outliers.begin(),outliers.end(),*it);
         if(dup_it != outliers.end())
         {
            outliers.erase(dup_it);
         }
      }
   }
}

