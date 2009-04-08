/*
 **************************************************************************
 *                                                                        *
 *                           Open Bloom Filter                            *
 *                                                                        *
 * Description: Usage pattern of Compressible Bloom Filter                *
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
#include <deque>
#include <set>
#include <string>

#include "bloom_filter.hpp"


template <class T,
          class Allocator,
          template <class,class> class Container>
void read_file(const std::string& file_name, Container<T, Allocator>& c);

void generate_outliers(const std::vector<std::string>& word_list, std::deque<std::string>& outliers);
void purify_outliers(const std::vector<std::string>& word_list,std::deque<std::string>& outliers);

int main()
{
   std::vector<std::string> word_list;
   std::deque<std::string> outliers;

   std::cout << "Loading list....";
   read_file("word-list.txt",word_list);
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

   unsigned int random_seed = 0xA57EC3B2;
   std::size_t total_false_positive = 0;

   std::size_t word_list_storage_size = 0;
   for(unsigned int i = 0; i < word_list.size(); ++i)
   {
      word_list_storage_size += word_list[i].size();
   }

   std::size_t total_number_of_queries = 0;
   double desired_probability_of_false_positive = 1.0 / (1.0 * word_list.size());

   compressible_bloom_filter filter(word_list.size(),desired_probability_of_false_positive,random_seed);

   filter.insert(word_list.begin(),word_list.end());

   while (filter.size() > 1)
   {
      std::vector<std::string>::iterator it = filter.contains_all(word_list.begin(),word_list.end());
      if (word_list.end() != it)
      {
         std::cout << "ERROR: key not found! =>" << (*it) << std::endl;
         return 1;
      }

      for(std::deque<std::string>::iterator it = outliers.begin(); it != outliers.end(); ++it)
      {
         if (filter.contains(*it)) ++total_false_positive;
      }

      total_number_of_queries += (outliers.size() + word_list.size());
      double pfp = total_false_positive / (1.0 * total_number_of_queries);

      std::cout << "Filter Size: "                    << filter.size() << "\t";
      std::cout << "EFPP: " << filter.effective_fpp() << "\t";
      std::cout << "OFPP: " << pfp                    << "\t";
      std::cout << "Diff: " << 100.0 * (pfp / filter.effective_fpp()) << "%" << std::endl;

      if (!filter.compress(5.0))
      {
         std::cout << "Filter cannot be compressed any further." << std::endl;
         break;
      }
   }

   return 0;
}


template <class T,
          class Allocator,
          template <class,class> class Container>
void read_file(const std::string& file_name, Container<T, Allocator>& c)
{
   std::ifstream stream(file_name.c_str());
   if (!stream) return;
   std::string buffer;
   while(std::getline(stream,buffer)) { c.push_back(buffer); }
}


std::string reverse(std::string str)
{
   // Not the most efficient way of doing this.
   std::reverse(str.begin(),str.end());
   return str;
}

void generate_outliers(const std::vector<std::string>& word_list, std::deque<std::string>& outliers)
{
   for(std::vector<std::string>::const_iterator it = word_list.begin(); it != word_list.end(); ++it)
   {
      if ((*it) != reverse((*it)))
      {
         outliers.push_back((*it) + reverse((*it)));
         outliers.push_back((*it) + (*it));
         outliers.push_back(reverse((*it)) + (*it) + reverse((*it)));
      }

      std::string ns = *it;
      for(unsigned int i = 0; i < ns.size(); ++i)
      {
         if (1 == (i & 0x00)) ns[i] = ~ns[i];
      }
      outliers.push_back(ns);
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

void purify_outliers(const std::vector<std::string>& word_list, std::deque<std::string>& outliers)
{
   std::set<std::string> set1;
   std::set<std::string> set2;

   std::copy(word_list.begin(),word_list.end(),std::inserter(set1,set1.begin()));
   std::copy(outliers.begin(),outliers.end(),std::inserter(set2,set2.begin()));

   std::deque<std::string> intersect_list;

   std::set_intersection(set1.begin(),set1.end(),
                         set2.begin(),set2.end(),
                         std::back_inserter(intersect_list));

   std::sort(intersect_list.begin(),intersect_list.end());

   if(!intersect_list.empty())
   {
      std::deque<std::string> new_outliers;
      for(std::deque<std::string>::iterator it = outliers.begin(); it != outliers.end(); ++it)
      {
         std::deque<std::string>::iterator dup_it = std::lower_bound(intersect_list.begin(),intersect_list.end(),*it);
         if(dup_it == intersect_list.end())
            new_outliers.push_back(*it);
      }
      outliers.swap(new_outliers);
   }
}
