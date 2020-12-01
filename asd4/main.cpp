#include "RedBlackTree.h"
#include <string>
#include <fstream>

#include <conio.h>

struct DictEntry
{
	DictEntry()
		: key(""), value("") {}
	DictEntry(const std::string& keyVal, const std::string& dictVal)
		: key(keyVal), value(dictVal) {}

	std::string key;
	std::string value;

	bool empty()
	{
		return key == "";
	}

	friend bool operator==(const DictEntry& d1, const DictEntry& d2)
	{
		return d1.key == d2.key;
	}

	friend bool operator==(const DictEntry& d, const std::string& str)
	{
		return d.key == str;
	}

	friend bool operator<(const DictEntry& d1, const DictEntry& d2)
	{
		return d1.key < d2.key;
	}

	friend std::ostream& operator<<(std::ostream& os, const DictEntry& entry)
	{
		os << entry.key << "-" << entry.value;
		return os;
	}
};

template<typename DictEntry>
const DictEntry& RedBlackTree<DictEntry>::operator[] (const std::string& key) const
{
	return Find(key);
}

template<typename DictEntry>
const DictEntry& RedBlackTree<DictEntry>::Find(const std::string& key) const
{
	return Find(DictEntry(key, ""));
}

const char* menu =
R"(===================
1. Wstaw slowo
2. Usun slowo
3. Wyszukaj slowo
4. Wczytaj z pliku
5. Zapisz do pliku
0. Zakoncz
===================)";


class App
{
public:
	void Loop();

	const RedBlackTree<DictEntry>& GetTree() const { return mTree; }

private:
	RedBlackTree<DictEntry> mTree;
	bool isRunning = true;

	void ReadFromFile(const std::string& path);
	void WriteToFile(const std::string& path);
	void PrintTree();
};


int main()
{
	App* app = new App();
	app->Loop();
	delete app;
	return 0;
}

void App::Loop()
{	
	DictEntry tmp;
	std::string buf;
	char e;
	while (isRunning)
	{
		std::cout << menu << std::endl;
		e = _getch();
		system("cls");
		switch (e)
		{
		case '1':
			std::cout << "Dodaj slowo" << std::endl;
			std::cout << "Podaj angielskie slowo: ";
			std::cin >> tmp.key;
			std::cout << "Podaj tlumaczenie: ";
			std::cin >> tmp.value;
			mTree.Insert(tmp);
			break;
		case '2':
			std::cout << "Usun slowo" << std::endl;
			std::cout << "Podaj angielskie slowo: ";
			std::cin >> buf;
			if (!mTree.Remove(DictEntry(buf, "")))
				std::cout << "Brak slowa " << buf << std::endl;
			else
				std::cout << "Usunieto slowo " << buf << std::endl;
			break;
		case '3':
			std::cout << "Wyszukaj slowo" << std::endl;
			std::cout << "Podaj angielskie slowo: ";
			std::cin >> buf;
			tmp = mTree.Find(buf);
			if (!tmp.empty())
				std::cout << tmp << std::endl;
			else
				std::cout << "Brak tlumaczen slowa " << buf << std::endl;
			break;
		case '4':
			std::cout << "Wczytaj z pliku" << std::endl;
			std::cout << "Podaj sciezke: ";
			std::cin >> buf;
			ReadFromFile(buf);
			break;
		case '5':
			std::cout << "Zapisz do pliku" << std::endl;
			std::cout << "Podaj sciezke: ";
			std::cin >> buf;
			if(!mTree.empty())
				WriteToFile(buf);
			break;
		case '0': 
			isRunning = false; 
			break;
		}
		std::cout << "Wcisnij dowolny klawisz aby kontynuowac.";
		_getch();
		system("cls");
	}	
}

void App::ReadFromFile(const std::string& path)
{
	std::fstream file;
	std::string left, right;
	file.open(path, std::ios::in);
	while (!file.eof())
	{
		std::getline(file, left, '\0');
		std::getline(file, right, '\0');
		if (left != "" && right != "")
			mTree.Insert(DictEntry(left, right));
	}
	file.close();
}

void App::WriteToFile(const std::string& path)
{
	std::fstream file;
	file.open(path, std::ios::out | std::ios::binary);
	for (auto a = mTree.begin(); a != mTree.end(); ++a)
	{
		file.write((*a).key.c_str(), (*a).key.size() + 1);
		file.write((*a).value.c_str(), (*a).value.size() + 1);
	}
	file.close();
}

void App::PrintTree()
{
	for (auto a = mTree.begin(); a != mTree.end(); ++a)
	{
		std::cout << *a << std::endl;
	}
}
