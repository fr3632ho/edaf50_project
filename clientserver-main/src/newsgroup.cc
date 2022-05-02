#include "newsgroup.h"
#include <vector>
#include <string>

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
