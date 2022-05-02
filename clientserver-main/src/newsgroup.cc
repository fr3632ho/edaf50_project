#include "newsgroup.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <iostream>

using std::map;
using std::string;
using std::vector;

Newsgroup::Newsgroup(string ng_title, int ng_id) : title(ng_title), id(ng_id) {}

string Newsgroup::getTitle()
{
    return title;
}

int Newsgroup::getId()
{
    return id;
}

map<int, Article> Newsgroup::getArticles()
{
    return articles;
}

void Newsgroup::deleteArticle(int articleId)
{
    articles.erase(articleId);
}

void Newsgroup::writeArticle(string title, string text, string author)
{
    articles.insert(std::pair<int, Article>(article_id, Article(title, author, text, article_id)));
    article_id++;
}
