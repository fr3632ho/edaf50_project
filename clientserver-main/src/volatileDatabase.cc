#include "volatileDatabase.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

VolatileDatabase::VolatileDatabase() {}

void VolatileDatabase::createNewsgroup(string title)
{
    newsgroups.push_back(title);
}

vector<string> VolatileDatabase::getNewsgroups()
{
    return newsgroups;
}

string VolatileDatabase::deleteNewsgroup(int newsgroupId)
{
    return "";
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
