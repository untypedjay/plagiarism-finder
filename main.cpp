// https://en.cppreference.com/w/cpp/container/map
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <map>

//using index_map_t = std::map <std::string, std::set <int>>;
using index_set_t = std::set<std::string>;
//using index_map_pair_t = index_map_t::value_type;

const size_t MAX_SHORT_WORD_LENGTH = 3;
const size_t NUMBER_OF_INPUT_FILES = 2;

const std::vector<std::string> english_stopwords = {"about", "above", "after", "again", "against", "arent", "because", "been",
                                              "before", "being", "below", "between", "both", "cant", "cannot", "could",
                                              "couldnt", "didnt", "does", "doesnt", "doing", "dont", "down", "during",
                                              "each", "from", "further", "hadnt", "hasnt", "have", "havent", "having",
                                              "hed", "hell", "hes", "here", "heres", "hers", "herself", "himself",
                                              "hows", "into", "isnt", "itself", "lets", "more", "most", "mustnt",
                                              "myself", "once", "only", "other", "ought", "ours", "ourselves", "over",
                                              "same", "shant", "shed", "shell", "shes", "should", "shouldnt", "some",
                                              "such,", "than", "that", "thats", "their", "theirs", "them", "themselves",
                                              "then", "there", "theres", "these", "they", "theyd", "theyll", "theyre",
                                              "theyve", "this", "those", "through", "under", "until", "very", "wasnt",
                                              "well", "were", "weve", "werent", "what", "whats", "when", "whens",
                                              "where", "wheres", "which", "while", "whos", "whom", "whys", "with",
                                              "wont", "would", "wouldnt", "youd", "youll", "youre", "youve", "your",
                                              "yours", "yourself", "yourselves"};

const std::vector<std::string> german_stopwords = {"aber", "auch", "bist", "dadurch", "daher", "darum", "dass", "dein",
                                             "deine", "dessen", "deshalb", "dies", "dieser", "dieses", "doch", "dort",
                                             "durch", "eine", "einem", "einen", "einer", "eines", "euer", "eure",
                                             "hatte", "hatten", "hattest", "hattet", "hier", "hinter", "ihre", "jede",
                                             "jedem", "jeden", "jeder", "jedes", "jener", "jenes", "jetzt", "kann",
                                             "kannst", "können", "könnt", "machen", "mein", "meine", "mußt", "musst",
                                             "müssen", "müßt", "nach", "nachdem", "nein", "nicht", "oder", "seid",
                                             "sein", "seine", "sich", "sind", "soll", "sollen", "sollst", "sollt",
                                             "sonst", "soweit", "sowie", "unser", "unsere", "unter", "wann", "warum",
                                             "weiter", "weitere", "wenn", "werde", "werden", "werdet", "weshalb",
                                             "wieder", "wieso", "wird", "wirst", "woher", "wohin", "über"};

// reads chars from an input stream and stores them in a string
std::string get_line(std::istream &in) {
    std::string str;
    std::getline(in, str);
    return str;
}

// erase numbers and punctuations from a string
std::string purify(std::string str) {
    str.erase(std::remove_if(std::begin(str), std::end(str), [] (auto const c) {
        return std::isdigit(c, std::locale::classic()) ||
               std::ispunct(c, std::locale::classic());
    }), str.end());
    return str;
}

// returns a string in lower case
std::string to_lower(std::string str) {
    std::transform(std::begin(str), std::end(str), std::begin(str), [] (auto &c) {
        return std::tolower(c, std::locale::classic());
    });
    return str;
}

std::string remove_stopwords(std::string str) {
    for(auto const &stopword : english_stopwords) {
        if (str == stopword) {
            return "";
        }
    }
    for(auto const &stopword : german_stopwords) {
        if (str == stopword) {
            return "";
        }
    }
    return str;
}

std::string remove_short_words(std::string str) {
    if (str.length() <= MAX_SHORT_WORD_LENGTH) {
        return "";
    } else {
        return str;
    }
}

// deletes digits and puctuations and returns string in lower case
std::string normalize(std::string str) {
    return remove_short_words(remove_stopwords(to_lower(purify(std::move(str)))));
}

template <typename T> class range {
public:
    using itor_t = T; // template parameter is not a real type name

    explicit range(itor_t b, itor_t e) : m_begin {b}, m_end {e} {

    }

    itor_t begin() {
        return m_begin;
    }

    itor_t end() {
        return m_end;
    }

private:
    itor_t m_begin {};
    itor_t m_end   {};
};

template <typename T> range <T> make_range(T b, T e) { // it's because we don't want to specify the T
    return range <T> {b, e};
}

template <typename T> auto make_range(std::istream &in) { // we also want a range from a stream
    return make_range(std::istream_iterator <T> {in}, {}); // this T are the objects the stream is iterating over (here: words)
}

struct line_string : std::string {
    friend auto &operator >> (std::istream &lhs, line_string &rhs) {
        return std::getline(lhs, rhs);
    }
};

void add_to_index(std::ifstream &file, index_set_t &index) {
    // loop through all lines
    for (auto const &line : make_range <std::string> (file)) { // everything that has a begin and an end can be on the the right side of the colon
        std::istringstream line_in {line};

        // loop through all words of a line
        for (std::string const &word : make_range <std::string> (line_in)) {
            // insert word into index
            if (auto w {normalize(word)}; !std::empty(w)) { // variable only scoped in if statement
                index.insert(w);
            }
        }
    }
}

void print_index(index_set_t index) {
    // loop through each entry in the index
//    for (auto const &word : index) {
//        std::cout << word << "\n";
//    }
    std::cout << "======================\n";
    std::cout << "Total: " << index.size() << " words.\n";
}

void print_result(double first, double second, double intersection) {
    double degree_of_conformity = 0;
    if (first && second && intersection) {
        std::cout << "intersection: " << intersection << ", first: " << first << "\n";
        std::cout << "intersection / first: " << intersection / first << "\n";
        degree_of_conformity = (double)((((intersection / first) + (intersection / second))) / NUMBER_OF_INPUT_FILES) * 100;
    }
    std::cout << "Number of significant words matching: " << intersection << "\n";
    std::cout << "Degree of conformity:\n";
    std::cout << "[";
    for (size_t i = 0; i < (size_t)degree_of_conformity; ++i) {
        std::cout << "#";
    }
    for (size_t i = (size_t)degree_of_conformity; i < 100; ++i) {
        std::cout << "-";
    }
    std::cout << "] " << degree_of_conformity << "%\n";
}

void run_without_word_weightening(std::ifstream &first, std::ifstream &second) {
    index_set_t index1 {};
    index_set_t index2 {};
    index_set_t result_index {};

    add_to_index(first, index1);
    add_to_index(second, index2);

    std::set_intersection(index1.begin(), index1.end(), index2.begin(), index2.end(), std::inserter(result_index, result_index.end()));
    print_result(index1.size(), index2.size(), result_index.size());
}

int main(int argc, char *argv[]) {
    std::ifstream first_file = (std::ifstream)argv[1];
    std::ifstream second_file = (std::ifstream) argv[2];
    run_without_word_weightening(first_file, second_file);
}
