#include "article.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

Article::Article(string title, string author, string text, int id) : author(author), text(text), title(title), id(id) {}

string Article::getTitle()
{
    return title;
}

string Article::getAuthor()
{
    return author;
}

string Article::getText()
{
    return text;
}

int Article::getId()
{
    return id;
}
