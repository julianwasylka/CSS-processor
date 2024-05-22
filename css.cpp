#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

using namespace std;

const int MAX_LENGTH = 1000;
const int T = 8;

char* cut(char* str)
{
    int start = 0;
    int end = (int)(strlen(str)) - 1;

    while (isspace(str[start]))
    {
        start++;
    }
    while (end > start && isspace(str[end]))
    {
        end--;
    }
    memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';

    return str;
}

struct Attribute
{
    char* property;
    char* value;
    struct Attribute* next = nullptr;
    void free()
    {
        delete[] property;
        delete[] value;
    }
    void freeAll()
    {
        this->free();
        Attribute* tmp = next;
        Attribute* old = nullptr;
        if (tmp == nullptr) return;
        while (tmp != nullptr)
        {
            old = tmp;
            tmp = tmp->next;
            old->free();
            delete old;
        }
    }
};

struct Selector
{
    char* name;
    struct Selector* next = nullptr;
    void free()
    {
        delete[] name;
    }
    void freeAll()
    {
        this->free();
        Selector* tmp = next;
        Selector* old = nullptr;
        if (tmp == nullptr) return;
        while (tmp != nullptr)
        {
            old = tmp;
            tmp = tmp->next;
            old->free();
            delete old;
        }
    }
};

struct Section
{
    Selector* firstSelector = nullptr;
    void addSelector(const char* name)
    {
        Selector* newSelector = new Selector;
        newSelector->name = new char[strlen(name) + 1];
        strcpy(newSelector->name, name);
        newSelector->next = nullptr;

        if (firstSelector == nullptr)
        {
            firstSelector = newSelector;
        }
        else
        {
            Selector* tmp = firstSelector;
            if (strcmp(tmp->name, name) == 0)
            {
                newSelector->free();
                delete newSelector;
                return;
            }
            while (tmp->next != nullptr)
            {
                tmp = tmp->next;
                if (strcmp(tmp->name, name) == 0)
                {
                    newSelector->free();
                    delete newSelector;
                    return;
                }
            }
            tmp->next = newSelector;
        }
    }
    char* getAtPosSel(int pos)
    {
        Selector* tmp = firstSelector;
        while (tmp != nullptr)
        {
            if (pos == 0) return tmp->name;
            pos--;
            tmp = tmp->next;
        }
        return nullptr;
    }
    Selector* findSelector(const char* name)
    {
        Selector* tmp = firstSelector;
        while (tmp != nullptr)
        {
            if (strcmp(tmp->name, name) == 0) return tmp;
            tmp = tmp->next;
        }
        return nullptr;
    }
    int const getSelLen()
    {
        int counter = 0;
        Selector* tmp = firstSelector;
        if (tmp == nullptr) return counter;
        while (tmp->next != nullptr)
        {
            counter++;
            tmp = tmp->next;
        }
        counter++;
        return counter;
    }
    ////////////////////////////////////////////////////////////
    Attribute* firstAttribute = nullptr;
    void addAttr(const char* prop, const char* val)
    {
        Attribute* newAttr = new Attribute;
        newAttr->property = new char[strlen(prop) + 1];
        strcpy(newAttr->property, prop);
        newAttr->value = new char[strlen(val) + 1];
        strcpy(newAttr->value, val);
        newAttr->next = nullptr;

        if (firstAttribute == nullptr)
        {
            firstAttribute = newAttr;
        }
        else
        {
            Attribute* tmp = firstAttribute;
            if (strcmp(tmp->property, prop) == 0)
            {
                if (tmp->next == nullptr)
                {
                    Attribute* old = tmp;
                    firstAttribute = newAttr;
                    old->free();
                    delete old;
                    return;
                }
                else
                {
                    Attribute* old = tmp;
                    firstAttribute = tmp->next;
                    old->free();
                }
            }
            Attribute* prev = tmp;
            while (tmp->next != nullptr)
            {
                if (strcmp(tmp->property, prop) == 0)
                {
                    prev->next = tmp->next;
                }
                prev = tmp;
                tmp = tmp->next;
            }
            if (strcmp(tmp->property, prop) == 0)
            {
                prev->next = newAttr;
                Attribute* old = tmp;
                old->free();
                delete old;
                return;
            }
            tmp->next = newAttr;
        }
    }
    int const getAttrLen()
    {
        int counter = 0;
        Attribute* tmp = firstAttribute;
        if (tmp == nullptr) return counter;
        while (tmp->next != nullptr)
        {
            counter++;
            tmp = tmp->next;
        }
        counter++;
        return counter;
    }
    Attribute* findAttr(const char* prop)
    {
        Attribute* tmp = firstAttribute;
        while (tmp != nullptr)
        {
            if (strcmp(tmp->property, prop) == 0) return tmp;
            tmp = tmp->next;
        }
        return nullptr;
    }
    char* findValue(const char* prop)
    {
        Attribute* tmp = firstAttribute;
        while (tmp != nullptr)
        {
            if (strcmp(tmp->property, prop) == 0) return tmp->value;
            if (tmp->next == nullptr) return nullptr;
            tmp = tmp->next;
        }
        return nullptr;
    }
    void removeAttr(char* prop)
    {
        if (firstAttribute == nullptr) return;
        if (strcmp(firstAttribute->property, prop) == 0)
        {
            if (firstAttribute->next == nullptr)
            {
                firstAttribute->free();
                delete firstAttribute;
                firstAttribute = nullptr;
                return;
            }
            Attribute* old = firstAttribute;
            firstAttribute = firstAttribute->next;
            old->free();
            delete old;
            return;
        }
        Attribute* tmp = firstAttribute;
        while (tmp->next != nullptr)
        {
            if (strcmp(tmp->next->property, prop) == 0)
            {
                Attribute* old = tmp->next;
                old->free();
                tmp->next = tmp->next->next;
                delete old;
                return;
            }
            tmp = tmp->next;
        }
    }
    void free()
    {
        if (firstAttribute != nullptr) firstAttribute->freeAll();
        if (firstSelector != nullptr) firstSelector->freeAll();
    }
};

struct Block
{
    Section* sections[T] = { nullptr };
    int usage = 0;
    int actualUsage = 0;
    struct Block* prev = nullptr;
    struct Block* next = nullptr;

    bool addSection(Section* section)
    {
        if (usage < T)
        {
            sections[actualUsage] = section;
            usage++;
            actualUsage++;
            return true;
        }
        return false;
    }
    void removeSection(int pos)
    {
        Section* newSections[T];

        for (int i = 0; i < pos; i++) newSections[i] = sections[i];
        for (int j = pos; j < T - 1; j++) newSections[j] = sections[j + 1];
        sections[pos]->free();
        for (int k = 0; k < T; k++) sections[k] = newSections[k];
        actualUsage--;
    }
    Section* getSection(int pos)
    {
        return sections[pos];
    }
    int countSelector(char* sel)
    {
        int liczba = 0;
        for (int i = 0; i < actualUsage; i++)
        {
            if (sections[i] != nullptr)
            {
                if (sections[i]->findSelector(sel) != nullptr) liczba++;
            }
        }
        return liczba;
    }
    int countAttr(char* attr)
    {
        int liczba = 0;
        for (int i = 0; i < actualUsage; i++)
        {
            if (sections[i] != nullptr)
            {
                if (sections[i]->findAttr(attr) != nullptr) liczba++;
            }
        }
        return liczba;
    }
    void free()
    {
        for (int i = 0; i < actualUsage; i++)
        {
            if (sections[i] != nullptr) sections[i]->free();
        }
    }
};

struct BlockList
{
    Block* firstBlock = nullptr;
    Block* lastBlock = nullptr;
    void add_Section(Section* section)
    {
        if (section->getAttrLen() == 0) return;
        if (firstBlock == nullptr)
        {
            Block* blok = new Block;
            blok->addSection(section);
            firstBlock = blok;
            lastBlock = blok;
        }
        else
        {
            Block* tmp = firstBlock;
            while (tmp->next != nullptr)
            {
                tmp = tmp->next;
            }
            if (tmp->addSection(section) == false)
            {
                Block* newblok = new Block;
                newblok->addSection(section);
                newblok->prev = tmp;
                tmp->next = newblok;
                lastBlock = newblok;
            }
        }
    }
    bool remove_Section(int pos)
    {
        if (firstBlock == nullptr) return false;
        else
        {
            Block* tmp = firstBlock;
            while (pos >= tmp->actualUsage && tmp->next != nullptr)
            {
                pos -= tmp->actualUsage;
                tmp = tmp->next;
            }
            if (pos >= tmp->actualUsage || pos < 0) return false;
            tmp->removeSection(pos);
            if (tmp->actualUsage == 0)
            {
                if (tmp->next != nullptr) tmp->next->prev = tmp->prev;
                if (tmp->next == nullptr) lastBlock = tmp->prev;
                if (tmp->prev == nullptr) firstBlock = tmp->next;
                else tmp->prev->next = tmp->next;
                tmp->free();
                delete tmp;
            }
        }
        return true;
    }
    Section* get_Section(int pos)
    {
        if (firstBlock == nullptr) return nullptr;
        else
        {
            Block* tmp = firstBlock;
            while (pos >= tmp->actualUsage)
            {
                if (tmp->next == nullptr) return nullptr;
                pos -= tmp->actualUsage;
                tmp = tmp->next;
            }
            return tmp->getSection(pos);
        }
    }
    int getSelLengthForSec(int pos)
    {
        Section* section = get_Section(pos);
        if (section == nullptr) return -1;
        else
        {
            return section->getSelLen();
        }
    }
    int getAttrLengthForSec(int pos)
    {
        Section* section = get_Section(pos);
        if (section == nullptr) return -1;
        else
        {
            return section->getAttrLen();
        }
    }
    int const getBlockListLen()
    {
        int counter = 0;
        Block* tmp = firstBlock;
        if (tmp == nullptr) return counter;
        while (tmp->next != nullptr)
        {
            counter += tmp->actualUsage;
            tmp = tmp->next;
        }
        counter += tmp->actualUsage;
        return counter;
    }
    int const count_Selector(char* sel)
    {
        int wynik = 0;
        Block* tmp = firstBlock;
        if (tmp == nullptr) return wynik;
        if (tmp->next == nullptr) return tmp->countSelector(sel);
        while (tmp->next != nullptr)
        {
            wynik += tmp->countSelector(sel);
            tmp = tmp->next;
        }
        wynik += tmp->countSelector(sel);
        return wynik;
    }
    int const count_Attr(char* attr)
    {
        int wynik = 0;
        Block* tmp = firstBlock;
        if (tmp == nullptr) return wynik;
        if (tmp->next == nullptr) return tmp->countAttr(attr);
        while (tmp->next != nullptr)
        {
            wynik += tmp->countAttr(attr);
            tmp = tmp->next;
        }
        wynik += tmp->countAttr(attr);
        return wynik;
    }
    char* getE(char* z, char* n)
    {
        Block* tmp = lastBlock;
        Section* szukana = nullptr;
        if (tmp->prev == nullptr)
        {
            for (int i = 0; i < tmp->actualUsage; i++)
            {
                if (tmp->sections[i]->findSelector(z) != nullptr && tmp->sections[i]->findAttr(n) != nullptr) szukana = tmp->sections[i];
            }
            if (szukana != nullptr)
            {
                return szukana->findAttr(n)->value;
            }
        }
        while (tmp->prev != nullptr)
        {
            for (int i = 0; i < tmp->actualUsage; i++)
            {
                if (tmp->sections[i]->findSelector(z) != nullptr && tmp->sections[i]->findAttr(n) != nullptr) szukana = tmp->sections[i];
            }
            if (szukana != nullptr)
            {
                return szukana->findAttr(n)->value;
            }
            tmp = tmp->prev;
        }
        for (int i = 0; i < tmp->actualUsage; i++)
        {
            if (tmp->sections[i]->findSelector(z) != nullptr && tmp->sections[i]->findAttr(n) != nullptr) szukana = tmp->sections[i];
        }
        if (szukana != nullptr)
        {
            return szukana->findAttr(n)->value;
        }
        return nullptr;
    }
    void free()
    {
        Block* tmp = firstBlock;
        if (tmp == nullptr) return;
        while (tmp->next != nullptr)
        {
            Block* old = tmp;
            tmp = tmp->next;
            old->free();
            delete old;
        }
    }
};

//funckje do list wziete z prezentacji :)

struct Komenda
{
    char* inz;
    char* litera;
    char* jn;

    bool check()
    {
        return inz != nullptr && litera != nullptr && jn != nullptr;
    }

    void free()
    {
        delete[] inz;
        delete[] litera;
        delete[] jn;
    }
};

Komenda* split(char* str)
{
    Komenda* wynik = new Komenda;
    char delim = ',';
    int word = 0;
    int strSize = strlen(str);
    wynik->inz = new char[strSize];
    wynik->litera = new char[strSize];
    wynik->jn = new char[strSize];

    int inzSize = 0;
    int literaSize = 0;

    for (int i = 0; i < strSize; i++)
    {
        if (str[i] != delim)
        {
            if (word == 0)
            {
                wynik->inz[i] = str[i];
            }
            else if (word == 1)
            {
                inzSize = strlen(wynik->inz);
                wynik->litera[i - inzSize - 1] = str[i];
            }
            else if (word == 2)
            {
                literaSize = strlen(wynik->litera);
                wynik->jn[i - inzSize - literaSize - 2] = str[i];
            }
        }
        else
        {
            if (word == 0)
            {
                wynik->inz[i] = '\0';
                word = 1;
            }
            else if (word == 1)
            {
                wynik->litera[i - inzSize - 1] = '\0';
                word = 2;
            }
        }
    }
    wynik->jn[strSize - inzSize - literaSize - 2] = '\0';

    return wynik;
}

void command(char* str, BlockList* blok)
{
    if (strlen(str) == 0) return;
    if (strcmp(str, "?") == 0) cout << "? == " << blok->getBlockListLen() << endl;
    else
    {
        Komenda* com = split(str);
        if (!com->check()) return;
        char* szukana = nullptr;
        char litera = (com->litera[0]);
        switch (litera)
        {
        case 'S':
            if (strcmp(com->jn, "?") != 0)
            {
                int x, y;
                sscanf(com->inz, "%d", &x);
                sscanf(com->jn, "%d", &y);
                Section* sekcja = blok->get_Section(x - 1);
                if (sekcja != nullptr)
                {
                    szukana = sekcja->getAtPosSel(y - 1);
                }
            }
            else
            {
                if (com->inz[0] >= '0' && com->inz[0] <= '9')
                {
                    int x;
                    sscanf(com->inz, "%d", &x);
                    int wynik = blok->getSelLengthForSec(x - 1);
                    if (wynik != -1)
                    {
                        char text[50];
                        sprintf(text, "%d", wynik);
                        szukana = text;
                    }
                }
                else
                {
                    char* sel = cut(com->inz);
                    int wynik = blok->count_Selector(sel);
                    char text[50];
                    sprintf(text, "%d", wynik);
                    szukana = text;
                }
            }
            break;
        case 'A':
            if (strcmp(com->jn, "?") != 0)
            {
                int x;
                sscanf(com->inz, "%d", &x);
                Section* sekcja = blok->get_Section(x - 1);
                szukana = sekcja->findValue(cut(com->jn));
            }
            else
            {
                if (com->inz[0] >= '0' && com->inz[0] <= '9')
                {
                    int x;
                    sscanf(com->inz, "%d", &x);
                    int wynik = blok->getAttrLengthForSec(x - 1);
                    if (wynik != -1)
                    {
                        char text[50];
                        sprintf(text, "%d", wynik);
                        szukana = text;
                    }
                }
                else
                {
                    char* attr = cut(com->inz);
                    int wynik = blok->count_Attr(attr);
                    char text[50];
                    sprintf(text, "%d", wynik);
                    szukana = text;
                }
            }
            break;
        case 'E':
            szukana = blok->getE(cut(com->inz), cut(com->jn));
            break;
        case 'D':

            if (strcmp(com->jn, "*") == 0)
            {
                int x;
                sscanf(com->inz, "%d", &x);
                if (blok->remove_Section(x - 1))
                {
                    szukana = (char*)"deleted";
                }
            }
            else
            {
                int x;
                sscanf(com->inz, "%d", &x);
                Section* sekcja = blok->get_Section(x - 1);
                if (sekcja != nullptr)
                {
                    if (sekcja->findAttr(cut(com->jn)) != nullptr)
                    {
                        szukana = (char*)"deleted";
                        sekcja->removeAttr(cut(com->jn));
                        if (sekcja->getAttrLen() == 0) blok->remove_Section(x - 1);
                    }
                }
            }
            break;
        }
        if (szukana != nullptr)
        {
            cout << com->inz << "," << com->litera << "," << com->jn << " == " << szukana << endl;
        }
        com->free();
    }
}

int main()
{
    int c = ' ';
    char string[MAX_LENGTH];
    char value[MAX_LENGTH];
    BlockList* blok = new BlockList;
    while (c != EOF)
    {
        int i = 0;
        c = getchar();
        if (c == '?')
        {
            while (c != '\n')
            {
                c = getchar();
            }
            i = 0;
            memset(string, 0, MAX_LENGTH);
            c = getchar();
            while (c != EOF)
            {
                if (c == '\n')
                {
                    string[i] = '\0';
                    if (strcmp(cut(string), "****") == 0)
                    {
                        break;
                    }
                    command(cut(string), blok);
                    memset(string, 0, MAX_LENGTH);
                    i = 0;
                }
                string[i] = (char)c;
                i++;
                c = getchar();
            }
            string[i] = '\0';
            command(cut(string), blok);
        }
        else if (c != EOF)
        {
            if (isspace(c)) continue;
            Section* sekcja = new Section;
            i = 0;
            memset(string, 0, MAX_LENGTH);
            while (c != '{')
            {
                string[i] = (char)c;
                i++;
                c = getchar();
                if (c == ',')
                {
                    string[i] = '\0';
                    sekcja->addSelector(cut(string));
                    memset(string, 0, MAX_LENGTH);
                    i = 0;
                    c = getchar();
                }
            }
            if (i != 0)
            {
                string[i] = '\0';
                sekcja->addSelector(cut(string));
                memset(string, 0, MAX_LENGTH);
                i = 0;
            }
            c = getchar();
            while (c != '}')
            {
                if (c == ';')
                {
                    i = 0;
                    c = getchar();
                }
                else if (c == ':')
                {
                    string[i] = '\0';
                    c = getchar();
                    i = 0;
                    while (c != ';' && c != '}')
                    {
                        value[i] = (char)c;
                        i++;
                        c = getchar();
                    }
                    value[i] = '\0';
                    sekcja->addAttr(cut(string), cut(value));
                    memset(string, 0, MAX_LENGTH);
                    memset(value, 0, MAX_LENGTH);
                }
                else
                {
                    string[i] = (char)c;
                    i++;
                    c = getchar();
                }
            }
            blok->add_Section(sekcja);
        }
    }
    blok->free();
    delete blok;
    return 0;
}
