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
 * http://www.opensource.org/licenses/cpl1.0.php                          *
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

   std::cout << "Loading list....";
   //read_file("word-list.txt",word_list);
   //read_file("word-list-large.txt",word_list);
   //read_file("word-list-extra-large.txt",word_list);
   //read_file("random-list.txt",word_list);
   std::cout << " Complete." << std::endl;

   if (word_list.empty())
   {
      std::cout << "ERROR: Input file invalid!" << std::endl;
      return 1;
   }

   generate_outliers(word_list,outliers);
   purify_outliers(word_list,outliers);

   unsigned int random_seed = 0;
   double total_false_positive = 0.0;

   std::size_t word_list_storage_size = 0;
   for(unsigned int i = 0; i < word_list.size(); ++i)
   {
      word_list_storage_size += word_list[i].size();
   }

   std::size_t total_numnber_of_quries = 0;
   while(random_seed < 1000)
   {
      double probability_of_false_positive = 1.0 / (1.0 * word_list.size());

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
            //std::cout << "ERROR: key that does not exist found! => " << (*it) << std::endl;
            total_false_positive++;
         }
      }

      total_numnber_of_quries +=  (outliers.size() + word_list.size());
      double current_pfp = total_false_positive / (1.0 * total_numnber_of_quries);

      std::cout << "Round:" << random_seed <<
                   "\tQueries:" << total_numnber_of_quries  <<
                   "\tFPQ:"     << total_false_positive << //Queries with False Positives
                   "\tIPFP:"     << probability_of_false_positive <<
                   "\tPFP:"      << current_pfp <<
                   "\tDPFP:"     << current_pfp - probability_of_false_positive <<
                   "\tTvD:"      << (100.0 * filter.size()) / (bits_per_char * word_list_storage_size) << "%" << std::endl;
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

   static const std::size_t rand_str_size = 120;
   static const std::string rand_str[rand_str_size] =
                  {
                     "MJTtT","td3rC","A5JNR","1yL5B","rQnJk","jNKYF","CD0XD","pFLSG",
                     "fxO1a","CAjBE","ORk4e","0LERI","R7d0x","Qqd7v","6Kih5","9tTCB",
                     "yCg9U","D2Tv7","XpNHn","6zeFQ","BT2cs","WGhKW","zTv6B","TTPFk",
                     "XjNVX","pg9yW","4pKiZ","mQUhL","xrXzR","kVRm5","NSyC4","olXm9",
                     "UWkYy","8Ys6r","yd4Fl","5L4mB","nP3nH","f0DFb","glnQa","DlXQa",
                     "cQdH6","eBmIN","fDj6F","ezLow","C15vu","I2Z2j","BQgzg","eVBid",
                     "hn5TO","WZyQN","xXgsE","sL6nK","8DKD8","jcrbp","AcRak","h8N5o",
                     "LViwC","ThEKf","O7fd5","oN0Id","OM1m0","4OLiR","VIa8N","bJZFG",
                     "9j3rL","SzW0N","7m7pY","mY9bg","k1p3e","3OFm1","r45se","VYwz3",
                     "pDjXt","ZcqcJ","npPHx","hA3bw","w7lSO","jEmZL","1x3AZ","FN47G",
                     "kThNf","aC4fq","rzDwi","CYRNG","gCeuG","wCVqO","d1R60","bEauW",
                     "KeUwW","lIKhO","RfPv3","dK5wE","1X7qu","tRwEn","1c03P","GwHCl",
                     "CsJaO","zl4j1","e0aEc","Uskgi","rgTGR","jyR4g","Tt6l4","lRoaw",
                     "94ult","qZwBX","eYW8S","Qf6UH","AbV56","N1hJq","JIaVe","8LHEx",
                     "DeNbS","30I0a","hm6qw","3jcaO","4WkuA","mQ219","Gb81C","yx4HM"
                  };


   for(unsigned int i = 0; i < rand_str_size; ++i)
   {
      std::string s0 = rand_str[i];
      std::string s1 = rand_str[(i + 1) % rand_str_size];
      std::string s2 = rand_str[(i + 2) % rand_str_size];
      std::string s3 = rand_str[(i + 3) % rand_str_size];
      std::string s4 = rand_str[(i + 4) % rand_str_size];
      std::string s5 = rand_str[(i + 5) % rand_str_size];
      std::string s6 = rand_str[(i + 6) % rand_str_size];

      outliers.push_back(s0);
      outliers.push_back(s0 + s1);
      outliers.push_back(s0 + s2 + s4);
      outliers.push_back(s0 + s1 + s3);
      outliers.push_back(s0 + s1 + s2 + s3 + s4 + s5);

      outliers.push_back(reverse(s0));
      outliers.push_back(reverse(s0 + s1));
      outliers.push_back(reverse(s0 + s2 + s4));
      outliers.push_back(reverse(s0 + s1 + s3));
      outliers.push_back(reverse(s0 + s1 + s2 + s3 + s4 + s5));
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

