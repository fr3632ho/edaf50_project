#include "diskDatabase.h"
#include "protocol.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <random>
//#include <sys/stat.h>

using std::cout;
using std::endl;
using std::pair;
using std::string;
using std::system;
using std::to_string;
using std::vector;

namespace fs = std::filesystem;

const string idmap = "id_newsgroups_map.txt";

DiskDatabase::DiskDatabase(const string &path) : path(path)
{
    initDb();
}

void DiskDatabase::initDb()
{
    if (!fs::exists(path))
    {
        fs::create_directory(path);

        string dir = path;
        dir.append(idmap);
        cout << dir << endl;
        std::ofstream outfile(dir);
        // o << "Hello, World!" << endl;
        outfile.close();
    }
    else
    {
        populateDb();
    }

    // for testing
    // for (const auto &entry : id_newsgroup_map)
    // {
    //     cout << entry.first << " -> " << entry.second << endl;
    // }
}

void DiskDatabase::populateDb()
{
    int max_id = 1;
    string line, delimiter = " ", token;
    std::ifstream file(path + idmap);
    while (getline(file, line))
    {
        vector<string> tokens;
        size_t pos = line.find(delimiter);
        int id = stoi(line.substr(0, pos));
        if (id > max_id)
        {
            max_id = id;
        }
        string name = line.substr(pos + 1, line.length());
        id_newsgroup_map.emplace(id, name);
    }
    this->id = max_id + 1;
}

// TODO: Hantera "newsgroup already exists"
bool DiskDatabase::createNewsgroup(string title)
{

    string ng_dir = path + to_string(id);
    for (auto it = id_newsgroup_map.begin(); it != id_newsgroup_map.end(); ++it)
    {
        if (it->second == title)
        {
            return false;
        }
    }

    if (!fs::exists(ng_dir))
    {
        std::ofstream outfile;
        outfile.open(path + idmap, std::ios_base::app);
        outfile << id << " " << title << endl;
        id_newsgroup_map.emplace(id, title);
        outfile.close();
        id++;

        return fs::create_directory(ng_dir);
    }
    return false;
}

map<int, Newsgroup> DiskDatabase::getNewsgroups()
{
    map<int, Newsgroup> groups;

    for (const auto &folder : fs::directory_iterator(path))
    {
        try
        {
            string f_path = folder.path().string();
            string f_name = f_path.substr(path.length(), f_path.length());
            int id = std::stoi(f_name);
            // cout << f_path << " -> " << f_name << endl;
            Newsgroup ng = getNewsgroup(id);
            groups.emplace(id, ng);
        }
        catch (const std::exception &e)
        {
            continue;
        }
    }

    return groups;
}

map<int, Article> DiskDatabase::getNewsgroupArticles(int newsgroupId)
{
    if (id_newsgroup_map.find(newsgroupId) == id_newsgroup_map.end())
    {
        // key 0 represents newsgroup not found
        map<int, Article> ng_not_found;
        ng_not_found.insert(std::pair<int, Article>(0, Article("", "", "", 0)));
        return ng_not_found;
    }
    Newsgroup ng = getNewsgroup(newsgroupId);
    return ng.getArticles();
}

Newsgroup DiskDatabase::getNewsgroup(int newsgroupId)
{
    Newsgroup ng(id_newsgroup_map[newsgroupId], newsgroupId);
    string n_dir = path + to_string(newsgroupId);
    if (fs::exists(n_dir))
    {
        for (const auto &article : fs::directory_iterator(n_dir))
        {
            string a_path = article.path().string();
            string a_name = a_path.substr(path.length() + to_string(newsgroupId).length() + 1, a_path.length());
            // cout << a_path << " -> " << a_name << endl;
            int a_id = std::stoi(a_name);
            Article a = getArticle(newsgroupId, a_id);
            ng.writeArticle(a);
        }
        return ng;
    }
    else
    {
        throw Protocol::ERR_NG_DOES_NOT_EXIST;
    }
}

Article DiskDatabase::getArticle(int newsgroupId, int articleId)
{
    string n_dir = path + to_string(newsgroupId);
    string a_path = n_dir + "/" + to_string(articleId);
    if (fs::exists(a_path))
    {
        string line;
        vector<string> lines;
        std::ifstream file(a_path);
        if (file.is_open())
        {
            while (getline(file, line))
            {
                lines.push_back(line);
            }
            file.close();
            string text = "";
            for (int i = 2; i < lines.size(); i++)
            {
                text += lines[i];
                if (i < lines.size() - 1)
                {
                    text += "\n";
                }
            }
            Article a(lines[0], lines[1], text, articleId);
            // cout << articleId << " -> (" << a.getAuthor() << "," << a.getTitle() << "," << a.getText() << ")" << endl;
            return a;
        }
    }
    else
    {
        throw Protocol::ERR_ART_DOES_NOT_EXIST;
    }
}

// changed to bool for error handling
bool DiskDatabase::writeArticle(int newsgroupId, string title, string text, string author)
{
    if (id_newsgroup_map.find(newsgroupId) == id_newsgroup_map.end())
    {
        return false;
    }
    string ng_dir = path + to_string(newsgroupId);
    // cout << "Using newsgroup_id " << newsgroupId << " on path: "<< path << endl;
    if (fs::exists(ng_dir))
    {
        int id = getNextArticleId(newsgroupId);
        string a_path = ng_dir + "/" + to_string(id);
        std::ofstream a_outfile(a_path);
        a_outfile << title << endl;
        a_outfile << author << endl;
        a_outfile << text << endl;
        a_outfile.close();
        return true;
    }

    // cout << "Could not find directory " << ng_dir << endl;
    return false;
}

int DiskDatabase::getNextArticleId(int newsgroupId)
{

    if (newsgroup_max_articleid.find(newsgroupId) != newsgroup_max_articleid.end())
    {
        newsgroup_max_articleid[newsgroupId] += 1;
        return newsgroup_max_articleid[newsgroupId];
    }

    string ng_dir = path + to_string(newsgroupId);
    int max_id = 0;
    if (fs::exists(ng_dir))
    {
        for (const auto &article : fs::directory_iterator(ng_dir))
        {
            string a_path = article.path().string();
            string a_name = a_path.substr(path.length() + to_string(id).length() + 1, a_path.length());
            int a_id = std::stoi(a_name);
            if (a_id > max_id)
            {
                max_id = a_id;
            }
        }
    }
    newsgroup_max_articleid[newsgroupId] = max_id + 1;
    return newsgroup_max_articleid[newsgroupId];
}

bool DiskDatabase::deleteNewsgroup(int newsgroupId)
{
    // Delete subdir with id
    string ng_dir = path + to_string(newsgroupId);

    if (!fs::remove_all(ng_dir))
    {
        return false;
    }

    id_newsgroup_map.erase(newsgroupId);

    std::ofstream idmap_outfile;
    idmap_outfile.open(path + idmap, std::ios::out);
    for (const auto &kv : id_newsgroup_map)
    {
        // cout << "Pairs: " << kv.first << " " << kv.second << endl;
        idmap_outfile << kv.first << " " << kv.second << endl;
    }
    idmap_outfile.close();
    return true;
}

// changed to bool for error handling
int DiskDatabase::deleteArticle(int newsgroupId, int articleId)
{
    if (id_newsgroup_map.find(newsgroupId) == id_newsgroup_map.end())
    {
        return 2;
    }
    string a_path = path + to_string(newsgroupId) + "/" + to_string(articleId);
    bool deleted = fs::remove(a_path);
    if (!deleted)
    {
        return 3;
    }
    return 1;
}
