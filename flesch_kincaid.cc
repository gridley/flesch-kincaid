// Counts words, syllables, and sentences in a text file, primarily for calculating
// the Flesch-Kincaid score of the text.

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

bool isvowel(char c) {
  switch (c) {
    case 'a':
      return true;
    case 'e':
      return true;
    case 'i':
      return true;
    case 'o':
      return true;
    case 'u':
      return true;
    case 'y':
      return true;
  }
  return false;
}

// http://jzimba.blogspot.com/2017/07/an-algorithm-for-counting-syllables.html
int n_syllables(std::string thisword) {
  int n_consonants = 0;
  int n_vowels = 0;
  unsigned size = thisword.size();

  for (char& c: thisword) {
    if (isvowel(c)) n_vowels++;
  }

  if (n_vowels == size) return size;

  int n_syl = 0;
  for (int i=0; i<size-1; ++i) {
    char first = thisword[i];
    char second = thisword[i+1];
    if (isvowel(first) and not isvowel(second)) n_syl++;
    if (first == 'i' and second == 'a') n_syl++;
    if (first == 'e' and second == 'o') n_syl++;
  }
  if (size > 4) {
    if (thisword.substr(size-4, 4) == "ious") n_syl++;
  }
  if (size > 3) {
    if (thisword.substr(size-3, 3) == "ier") n_syl++;
  }
  if (size > 2) {
    std::string ending = thisword.substr(size-2, 2);
    bool hasKeyEnding = false;
    if (ending == "ed") {
      hasKeyEnding = true;
      switch (thisword[size-3]) {
        case 'd':
          hasKeyEnding = false;
        case 't':
          hasKeyEnding = false;
        case 'r':
          if (size > 3) if (thisword[size-4] != 'r') hasKeyEnding = false;
        case 'l':
          if (size > 3) if (not isvowel(thisword[size-4])) hasKeyEnding = false;
      }
    }
    else if (ending == "es") if (size > 3) switch (thisword[size-3]) {
      case 'c':
        hasKeyEnding = false;
        break;
      case 'g':
        hasKeyEnding = false;
        break;
      case 'x':
        hasKeyEnding = false;
        break;
      case 's':
        hasKeyEnding = false;
        break;
      case 'z':
        hasKeyEnding = false;
        break;
      case 'l':
        if (size > 4) if (not isvowel(thisword[size-4])) hasKeyEnding = false;
        else hasKeyEnding = true;
        break;
      default:
        hasKeyEnding = true;
    }
    if (hasKeyEnding) n_syl--;
  }

  int n_final_syl = -1;

  if (size > 1) {
    std::string ending = thisword.substr(size-2, 2);
    if (ending == "ea")
      n_final_syl = 2;
    else if (ending == "ii")
      n_final_syl = 2;
    else if (ending == "io")
      n_final_syl = 2;
    else if (ending == "ua")
      n_final_syl = 2;
    else if (ending == "uo")
      n_final_syl = 2;
    else
      n_final_syl = 1;
  }
  if (thisword[size-1] == 'e')
    { if (size > 1) if (not isvowel(thisword[size-2])) n_final_syl = 0; }
  if (not isvowel(thisword[size-1])) n_final_syl = 0;
  if (n_final_syl == -1) n_final_syl = 1;

  n_syl += n_final_syl;

  if (n_syl < 1) n_syl = 1;

  return n_syl;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << "counttext filename" << endl << endl;
    cout << "This program counts the words, sentences, and syllables "
      "per word of a text file" << endl;
    exit(1);
  }

  ifstream f;
  f.open(argv[1]);

  if (not f.good()) { cerr << "Cannot open file " << argv[1] << endl; }

  string thisword;

  long unsigned n_sentences_total = 0;
  long unsigned n_words_total = 0;
  long unsigned n_syllables_total = 0;

  while(f >> thisword) {
    n_words_total++;

    auto period_loc = thisword.find('.');
    auto excl_loc = thisword.find('!');
    auto quest_loc = thisword.find('?');

    if (period_loc != string::npos || excl_loc != string::npos || quest_loc != string::npos) {
      n_sentences_total++;
    }

    // Convert word to all-alphanumeric and lowercase
    thisword.erase(remove_if(thisword.begin(),
          thisword.end(),
          [](char c) {return !isalpha(c); } ), thisword.end());
    for_each(thisword.begin(), thisword.end(), [](char& c){ c = tolower(c); });

    n_syllables_total += n_syllables(thisword);
  }

  double fk = 206.835 - 1.015 * n_words_total / n_sentences_total - 
    84.6 * n_syllables_total / n_words_total;
  double grade = -15.59 + .39 * n_words_total / n_sentences_total + 
    11.8 * n_syllables_total / n_words_total;
  std::cout << "Sentences: " << n_sentences_total << std::endl;
  std::cout << "Words: " << n_words_total << std::endl;
  std::cout << "Syllables: " << n_syllables_total << std::endl;
  std::cout << "Flesch-Kincaid score: " << fk << std::endl;
  std::cout << "Grade level: " << grade << std::endl;
}
