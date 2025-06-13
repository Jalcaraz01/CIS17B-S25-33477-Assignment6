#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>
#include <stdexcept>
#include <QString>
using namespace std;



class DuplicateItemException : public std::runtime_error {
private:
    string duplicate_error;
public:
    DuplicateItemException(const std::string& msg) : std::runtime_error(msg) {}
};

//This exception class defines an exception to be thrown in case of an item not being found in a collection
class ItemNotFoundException : public std::runtime_error {
public:
    ItemNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
};

class StoredItem {
private:
    std::string id;
    std::string description;
    std::string location;

public:
    StoredItem(std::string id, std::string desc, std::string loc)
        : id(id), description(desc), location(loc) {}

    std::string getId() const { return id; }
    std::string getDescription() const { return description; }
    std::string getLocation() const { return location; }
};

class StorageManager {
private:
    std::unordered_map<std::string, std::shared_ptr<StoredItem>> itemById;
    std::map<std::string, std::shared_ptr<StoredItem>> itemByDescription;

public:
    void addItem(const std::shared_ptr<StoredItem>& item) {
        //Adds item to both maps and throws exception if ID already exists
        try{
            auto it{itemById.cbegin()};
            while (it != itemById.cend()){
                if(it->first == item->getId()){
                    throw(DuplicateItemException("Item with ID " + item->getId() + " already exists!"));
                }
                ++it;
            }
            itemByDescription.insert(std::make_pair(item->getDescription(), item));
            itemById.insert(std::make_pair(item->getId(), item));
        }
        catch(const DuplicateItemException& Exception){
            cout << "Error: " << Exception.what() << endl;
        }
    }

    std::shared_ptr<StoredItem> findById(const std::string& id) const {
        //Returns item if found, if not, throws ItemNotFoundException and returns nullptr
        try{
            auto it{itemById.cbegin()};
            while (it != itemById.cend()){
                if (it->first == id){
                    return it->second;
                }
                ++it;
            }
            throw(ItemNotFoundException("Item with ID " + id + " not found!"));
        }
        catch(ItemNotFoundException& Exception){
            cout << "Error: " << Exception.what() << endl;

        }
        return nullptr;
    }

    void removeItem(const std::string& id) {
        //Removes item from both maps, throws exception if item ID is not found
        try{
            int counter = 0;
            auto it{itemById.cbegin()};
            while (it != itemById.cend()){
                if (it->first == id){
                    it = itemById.erase(it);
                    counter += 1;
                }
                ++it;
            }
            auto i{itemByDescription.cbegin()};
            while (i != itemByDescription.cend()){
                if ((i->second)->getId() == id){
                    i = itemByDescription.erase(i);
                    counter += 1;
                }
                ++i;
            }
            if (counter == 0){
                throw (ItemNotFoundException("Item with ID " + id + " not found!"));
            }
        }
        catch(ItemNotFoundException& Exception){
            cout << "Error: " << Exception.what() << endl;
        }
    }

    string listItemsByDescription() const {
        // Iterates over itemByDescription and displays item info in alphabetical description order
        string result = "";
        auto it{itemByDescription.cbegin()};
        while (it != itemByDescription.cend()){
            result += "- " + it->first;
            result += ": ID: " + (it->second)->getId();
            result += " Location: " + (it->second)->getLocation();
            result += "\n";
            ++it;
        }
        return result;
    }
};

StorageManager Storage;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

string ID, Desc, Loc;

void MainWindow::on_btnadditem_clicked()
{
    ID= ui->IDtxt->text().toStdString();
    Desc= ui->desctxt->text().toStdString();
    Loc= ui->Loctxt->text().toStdString();

  auto item1 {std::make_shared<StoredItem>(ID, Desc, Loc)};
    Storage.addItem(item1);
  string result = Storage.listItemsByDescription();
  ui->textEdit->setText(QString::fromStdString(result));
}


