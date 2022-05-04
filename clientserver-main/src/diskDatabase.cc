#include "diskDatabase.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <iostream>
#include <fstream>
//#include <sys/stat.h>


using std::pair;
using std::string;
using std::vector;
using std::system;
namespace fs = std::filesystem;

DiskDatabase::DiskDatabase(const string& path) : path(path)
{
    initDb();
}

void DiskDatabase::initDb() {
    if (!fs::exists(path)) {
        fs::create_directory(path);
        
        string dir = path;
        dir.append("idnewsgroupsmap.txt");
        std::cout << dir << std::endl;
        std::fflush(stdout);
        std::ofstream o(dir);
        o << "Hello, World!" << std::endl;
        o.close();
    }

}


void DiskDatabase::createNewsgroup(string title)
{

}

map<int, Newsgroup> DiskDatabase::getNewsgroups()
{

}

string DiskDatabase::deleteNewsgroup(int newsgroupId)
{
    // Delete subdir with id

    return "";
}

map<int, Article> DiskDatabase::getNewsgroupArticles(int newsgroupId)
{

    // get all articles in subdir with id

    // return ng.getArticles();
}

Article DiskDatabase::getArticle(int newsgroupId, int articleId)
{
    // get certain article with id

    // return newsgroups.at(newsgroupId).getArticles().at(articleId);
}

void DiskDatabase::writeArticle(int newsgroupId, string title, string text, string author)
{
    // write new article in subdir with id
}

void DiskDatabase::deleteArticle(int newsgroupId, int articleId)
{
    // Delete certain article
}
