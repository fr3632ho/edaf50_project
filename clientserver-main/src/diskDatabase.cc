#include "diskDatabase.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
//#include <sys/stat.h>


using std::pair;
using std::string;
using std::vector;

DiskDatabase::DiskDatabase(const string& path) : path(path)
{
    initDb();
}

void DiskDatabase::initDb() {
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directory(path);
    }
}


void DiskDatabase::createNewsgroup(string title)
{
}

map<int, Newsgroup> DiskDatabase::getNewsgroups()
{
    // Get all subdirs
    // return newsgroups;
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
