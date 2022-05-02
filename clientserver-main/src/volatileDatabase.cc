#include "volatileDatabase.h"
#include <vector>
#include <string>
#include <map>

using std::pair;
using std::string;
using std::vector;

VolatileDatabase::VolatileDatabase() {}

void VolatileDatabase::createNewsgroup(string title)
{
    ng.push_back(Newsgroup(title, id));
    newsgroups.insert(std::pair<int, Newsgroup>(id, Newsgroup(title, id)));
    id += 1;
}

map<int, Newsgroup> VolatileDatabase::getNewsgroups()
{
    return newsgroups;
}

string VolatileDatabase::deleteNewsgroup(int newsgroupId)
{
    newsgroups.erase(newsgroupId);
    return "Deleted";
}

string VolatileDatabase::getNewsgroupArticles(int newsgroupId)
{
    return "";
}

string VolatileDatabase::getArticle(int articleId)
{
    return "";
}

void VolatileDatabase::writeArticle(int articleId, string title, string text, string author)
{
}

void VolatileDatabase::deleteArticle(int articleId)
{
}
