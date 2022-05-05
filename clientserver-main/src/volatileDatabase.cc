#include "volatileDatabase.h"
#include "protocol.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <stdexcept>

using std::map;
using std::pair;
using std::string;
using std::vector;

VolatileDatabase::VolatileDatabase() {}

bool VolatileDatabase::createNewsgroup(string title)
{
    for (auto it = newsgroups.begin(); it != newsgroups.end(); ++it)
    {
        if (it->second.getTitle() == title)
        {
            return false;
        }
    }
    newsgroups.insert(std::pair<int, Newsgroup>(id, Newsgroup(title, id)));
    id += 1;
    return true;
}

map<int, Newsgroup> VolatileDatabase::getNewsgroups()
{
    return newsgroups;
}

bool VolatileDatabase::deleteNewsgroup(int newsgroupId)
{
    size_t erased = newsgroups.erase(newsgroupId);
    // 0 if nothing erased
    return erased;
}

map<int, Article> VolatileDatabase::getNewsgroupArticles(int newsgroupId)
{
    if (newsgroups.find(newsgroupId) == newsgroups.end())
    {
        // key 0 represents newsgroup not found
        map<int, Article> ng_not_found;
        ng_not_found.insert(std::pair<int, Article>(0, Article("", "", "", 0)));
        return ng_not_found;
    }
    Newsgroup &ng = newsgroups.at(newsgroupId);
    return ng.getArticles();
}

Article VolatileDatabase::getArticle(int newsgroupId, int articleId)
{
    if (newsgroups.find(newsgroupId) == newsgroups.end())
    {
        throw Protocol::ERR_NG_DOES_NOT_EXIST;
    }
    Newsgroup ng = newsgroups.at(newsgroupId);

    map<int, Article> articles = ng.getArticles();
    if (articles.find(articleId) == articles.end())
    {
        throw Protocol::ERR_ART_DOES_NOT_EXIST;
    }
    return articles.at(articleId);
}

bool VolatileDatabase::writeArticle(int newsgroupId, string title, string text, string author)
{
    if (newsgroups.find(newsgroupId) == newsgroups.end())
    {
        return false;
    }
    Newsgroup &ng = newsgroups.at(newsgroupId);
    ng.writeArticle(title, text, author);
    return true;
}

int VolatileDatabase::deleteArticle(int newsgroupId, int articleId)
{
    if (newsgroups.find(newsgroupId) == newsgroups.end())
    {
        return 2;
    }
    Newsgroup &ng = newsgroups.at(newsgroupId);
    bool deleted = ng.deleteArticle(articleId);
    if (!deleted)
    {
        return 3;
    }
    return 1;
}
