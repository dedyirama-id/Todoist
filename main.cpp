#include <iostream>
#include <string>
#include "src/hashtable.h"
#include "src/graph.h"
#include "src/binarytree.h"
#include "src/queue.h"
#include "src/account.h"
#include "src/note.h"
#include "src/app.h"
#include "src/utils.h"

using namespace std;

// Function Prototype
bool statusCheck(app::CliMenu menu, bool mustLoggedIn = true, bool mustHaveCommandValue = true);
void printHelp();
unsigned int usernameToId(const string &username);
Account *registerAccount();
Account *login();
void removeAccount();
void addTodo(const string &todo);
void addNotebook(const string &notebook);
void addNote(const string &note);
void openNotebook(const string &notebook);
void sendNotebook(const string &notebook);
void manageImport();
void printTodos();
void printTodoDetails(const int id);
void removeNote(const string &note);
void undoNotebookDelete();
void removeTodo(const int id);
void showWhatTodo();

// Global Object
ht::HashTable<Account> *accounts = new ht::HashTable<Account>();
Account *activeAccount = nullptr;
app::CliMenu menu({
  "exit",
  "help",
  "login",
  "logout",
  "au",
  "ru",
  "do",
  "cn",
  "an",
  "on",
  "sn",
  "import",
  "todos",
  "dt",
  "rn",
  "un",
  "rt",
  "wtd",
  "sid",
  "smp"
  });

int main() {
  system("cls");
  app::printH2("You need to create an account first!");
  registerAccount();

  system("cls");
  app::printH1("# Login");
  activeAccount = login();

  if (activeAccount == nullptr) {
    app::printError("Login failed!");
    return 0;
  }

  while (true) {
    system("cls");
    if (activeAccount == nullptr) app::printH1("# Programmers Notes: Not logged in");
    else app::printH1("# Programmers Notes: " + u::toUppercase(activeAccount->username));
    menu.getCommand();

    switch (menu.commandNum) {
    case 0: // exit
      break;

    case 1: // help
      printHelp();
      u::wait();
      break;

    case 2: // login
      if (activeAccount != nullptr) {
        app::printWarning("Already logged in!");
        u::wait();
        continue;
      }

      activeAccount = login();
      break;

    case 3: // logout
      if (activeAccount == nullptr) {
        app::printWarning("Not logged in!");
        u::wait();
        continue;
      }

      activeAccount = nullptr;
      app::printSuccess("Logged out!");
      u::wait();
      break;

    case 4: // au - add user
      if (!statusCheck(menu, false, false)) continue;
      app::printH2("Create new user account");
      registerAccount();
      break;

    case 5: // ru - remove user
      app::printH2("Remove user account");
      removeAccount();
      break;

    case 6: // do - add todo
      if (!statusCheck(menu, true, true)) continue;
      addTodo(menu.commandValue);
      break;

    case 7: // cn - create notebook
      if (!statusCheck(menu, true, true)) continue;
      addNotebook(menu.commandValue);
      break;

    case 8: // an - add note
      if (!statusCheck(menu, true, true)) continue;
      addNote(menu.commandValue);
      break;

    case 9: // on - open notebook
      if (!statusCheck(menu, true, true)) continue;
      openNotebook(menu.commandValue);
      break;

    case 10: // sn - send notebook
      if (!statusCheck(menu, true, true)) continue;
      sendNotebook(menu.commandValue);
      break;

    case 11: // import - manage notebook import
      if (!statusCheck(menu, true, true)) continue;
      manageImport();
      break;

    case 12: // todos
      if (!statusCheck(menu, true, false)) continue;
      printTodos();
      u::wait();
      break;

    case 13: // dt - print todo details
      if (!statusCheck(menu, true, true)) continue;
      try {
        printTodoDetails(stoi(menu.commandValue));
      } catch (...) {
        app::printWarning("Invalid todo id!");
        u::wait();
      }
      break;

    case 14: // rn - remove notebook
      if (!statusCheck(menu, true, true)) continue;
      if (menu.commandValue == "") {
        app::printWarning("Todo id cannot be empty!");
        app::printWarning("Try \"rn <title>\" to remove note!");
        u::wait();
        continue;
      }

      removeNote(menu.commandValue);
      break;

    case 15: // un - undo notebook delete
      if (!statusCheck(menu, true, true)) continue;
      undoNotebookDelete();
      break;

    case 16: // rm - remove todo
      if (!statusCheck(menu, true, true)) continue;
      try {
        removeTodo(stoi(menu.commandValue));
      } catch (...) {
        app::printWarning("Invalid todo id!");
        u::wait();
      }
      break;

    case 17: // wtd - show what to do
      if (!statusCheck(menu, true, true)) continue;
      showWhatTodo();
      u::wait();
      break;

    case 18: // sid - sort todo list by id
      if (!statusCheck(menu, true, true)) continue;
      app::printH2("Sort todo list by Id");
      activeAccount->todos->insertionSortById();
      printTodos();
      u::wait();
      break;

    case 19: // smp - sort todo list by most possible to do
      if (!statusCheck(menu, true, true)) continue;
      app::printH2("Sort todo list by most possible");
      activeAccount->todos->insertionSortByDegreeAscending();
      printTodos();
      u::wait();
      break;

    default:
      app::printWarning("Invalid command!");
      u::wait();
      break;
    };

    if (menu.commandNum == 0) break;
    menu.commandNum = -1;
  }
  return 0;
}

bool statusCheck(app::CliMenu menu, bool mustLoggedIn, bool mustHaveCommandValue) {
  if (mustLoggedIn && activeAccount == nullptr) {
    app::printWarning("You must login first!");
    u::wait();
    return false;
  }

  if (mustHaveCommandValue && menu.commandValue == "") {
    app::printWarning("Command value cannot be empty!");
    app::printWarning("Try \"<command> <value>\"!");
    cout << "For more info: type \"help\"" << endl;
    u::wait();
    return false;
  }

  return true;
}

void printHelp() {
  app::printH2("# Help");
  cout << "No help yet :(" << endl;
}

Account *registerAccount() {
  string username = u::getStringInput("Username: ");
  unsigned int id = usernameToId(username);

  if (accounts->getRecord(id) != nullptr) {
    app::printError("Username already exists!");
    return nullptr;
  }

  string password = u::getStringInput("Password: ");

  gr::Graph<string> *todos = new gr::Graph<string>();
  tr::BinaryTree<Note> *notes = new tr::BinaryTree<Note>();
  qu::Queue<Note> *notesQueue = new qu::Queue<Note>();
  st::Stack<Note> *notesStack = new st::Stack<Note>();
  Account newAccount(id, username, password);
  newAccount.todos = todos;
  newAccount.notes = notes;
  newAccount.notesQueue = notesQueue;
  newAccount.NotesStack = notesStack;

  accounts->addRecord(newAccount);
  return accounts->getRecord(id);
}

Account *login() {
  string username = u::getStringInput("Username: ");
  unsigned int id = usernameToId(username);

  Account *account = accounts->getRecord(id);
  if (account == nullptr) {
    app::printError("Account not found!");
    u::wait();
    return nullptr;
  }

  string password = u::getStringInput("Password: ");
  if (!account->validatePassword(password)) {
    app::printError("Invalid password!");
    return nullptr;
  }
  return account;
}

unsigned int usernameToId(const string &username) {
  unsigned int id = 0;
  for (int i = 0; i < username.size(); ++i) {
    id += (username[i] - 64) * u::pow(10, username.size() - i - 1);
  }
  return id;
}

void removeAccount() {
  string username = u::getStringInput("Username: ");
  unsigned int id = usernameToId(username);

  Account *account = accounts->getRecord(id);
  if (account == nullptr) {
    app::printError("Account not found!");
    u::wait();
    return;
  }

  accounts->removeRecord(id);
  app::printSuccess("Account removed!");
  u::wait();
}

void addTodo(const string &todo) {
  unsigned int todoId = activeAccount->todos->idCount + 1;

  gr::Vertex<string> *vertex = activeAccount->todos->getVerticesHead();
  while (vertex != nullptr) {
    if (vertex->data == todo) {
      app::printError("Todo already exists!");
      u::wait();
      return;
    }
    vertex = vertex->next;
  }

  bool success = activeAccount->todos->addVertex(todoId, todo);
  if (!success) {
    app::printError("Failed to add todo!");
    u::wait();
    return;
  }

  gr::Vertex<string> *vertexPrint = activeAccount->todos->getVerticesHead();
  if (activeAccount->todos->vertexCount <= 1) {
    app::printSuccess("Todo added!");
    activeAccount->todos->idCount++;
    u::wait();
    return;
  }

  while (true) {
    system("cls");
    app::printH1("# Todos Relationship: " + todo);
    app::printH2("ID Todo");
    while (vertexPrint != nullptr) {
      if (vertexPrint->id != todoId && vertexPrint->searchEdgeById(todoId) == false) cout << vertexPrint->id << ". " << vertexPrint->data << endl;

      vertexPrint = vertexPrint->next;
    }

    int todoIdInput = u::getIntInput("Todo id that depends on this: (0 to skip) ");
    if (todoIdInput == 0) break;

    if (activeAccount->todos->searchById(todoIdInput) == nullptr) {
      app::printError("Invalid todo id!");
      u::wait();
      continue;
    } else {
      activeAccount->todos->addEdgeById(todoIdInput, todoId);
      continue;
    }
  }

  activeAccount->todos->idCount++;
  app::printSuccess("Todo added!");
  u::wait();
}

void addNotebook(const string &notebook) {
  activeAccount->notes->insert(notebook, Note(notebook));
  app::printSuccess("Notebook added!");
  u::wait();
}

void addNote(const string &note) {
  app::printH2("Notebook available:");
  activeAccount->notes->inOrderTraversal();

  string notebook = u::getStringInput("Please select a notebook: ");

  if (activeAccount->notes->search(notebook) == nullptr) {
    app::printError("Notebook not found!");
    u::wait();
    return;
  }

  tr::Node<Note> *bookNode = activeAccount->notes->search(notebook);
  bookNode->data.addContent(note);
  app::printSuccess("Note added!");
  u::wait();
}

void openNotebook(const string &notebook) {
  tr::Node<Note> *bookNode = activeAccount->notes->search(notebook);
  if (bookNode == nullptr) {
    app::printError("Notebook not found!");
    u::wait();
    return;
  }

  app::printH2(bookNode->id);
  for (int i = 0; i < bookNode->data.count; ++i) {
    cout << "- " << bookNode->data.content[i] << endl;
  }
  u::wait();
}

void sendNotebook(const string &notebook) {
  tr::Node<Note> *bookNode = activeAccount->notes->search(notebook);
  if (bookNode == nullptr) {
    app::printError("Notebook not found!");
    u::wait();
    return;
  }

  string username = u::getStringInput("Send to user: ");
  unsigned int id = usernameToId(username);

  Account *targetAccount = accounts->getRecord(id);
  if (targetAccount == nullptr) {
    app::printError("User not found!");
    u::wait();
    return;
  }

  targetAccount->notesQueue->enqueue(bookNode->data);
  app::printSuccess("Note sent!");
  u::wait();
}

void manageImport() {
  app::Menu importMenu({ "Exit", "Accept", "Reject" });
  while (activeAccount->notesQueue->isEmpty() == false) {
    Note currentNote = activeAccount->notesQueue->peek();
    cout << currentNote.title << endl;

    for (int i = 0; i < currentNote.count; ++i) {
      cout << "- " << currentNote.content[i] << endl;
    }

    int choice = importMenu.getChoice();
    switch (choice) {
    case 0:
      return;
    case 1:
      activeAccount->notes->insert(currentNote.title, currentNote);
      activeAccount->notesQueue->dequeue();
      app::printSuccess("Note accepted!");
      break;
    case 2:
      activeAccount->notesQueue->dequeue();
      app::printError("Note rejected!");
      break;
    default:
      break;
    }
  }

  app::printWarning("Queue is empty!");
  u::wait();
}

void printTodos() {
  app::printH1("Todos");

  gr::Vertex<string> *vertexPrint = activeAccount->todos->getVerticesHead();
  app::printH2("ID  Todo");
  while (vertexPrint != nullptr) {
    cout << vertexPrint->id << ".  " << vertexPrint->data << endl;
    vertexPrint = vertexPrint->next;
  }
}

void printTodoDetails(const int id) {
  gr::Vertex<string> *current = activeAccount->todos->searchById(id);
  if (current == nullptr) {
    app::printError("Invalid todo id!");
    u::wait();
    return;
  }

  app::printH2("Todo Details");
  cout << "- Todo Id: " << current->id << endl;
  cout << "- Todo: " << current->data << endl;
  cout << "- Depends on: " << endl;
  gr::Edge<string> *edge = current->edgeList;
  while (edge != nullptr) {
    cout << "\t- [" << edge->vertexRef->id << "] " << edge->vertexRef->data << endl;
    edge = edge->next;
  }

  u::wait("\nEnter to continue...");
}

void removeNote(const string &title) {
  tr::Node<Note> *current = activeAccount->notes->search(title);
  if (current == nullptr) {
    app::printError("Invalid todo id!");
    u::wait();
    return;
  }

  Note toStack = current->data;
  activeAccount->NotesStack->push(toStack);
  activeAccount->notes->deleteNode(title);
  app::printSuccess("Note removed!");
  u::wait();
}

void undoNotebookDelete() {
  if (activeAccount->NotesStack->isEmpty()) {
    app::printError("Stack is empty!");
    u::wait();
    return;
  }

  Note currentNote = activeAccount->NotesStack->peek();
  activeAccount->notes->insert(currentNote.title, currentNote);
  activeAccount->NotesStack->pop();
  app::printSuccess("Note restored!");
  u::wait();
}

void removeTodo(const int id) {
  bool success = activeAccount->todos->deleteVertexById(id);
  if (!success) {
    app::printError("Todo not found!");
    u::wait();
    return;
  }
  app::printSuccess("Todo removed!");
  u::wait();
}

void showWhatTodo() {
  if (activeAccount->todos->isEmpty()) {
    app::printSuccess("Nothing to do! Todo list is empty!");
    u::wait();
    return;
  }

  activeAccount->todos->insertionSortByDegreeAscending();
  gr::Vertex<string> *todo = activeAccount->todos->getVerticesHead();
  int count = 1;
  app::printH2("No  Todo");
  while (todo != nullptr) {
    if (todo->degree > 0) break;
    cout << count << ".  " << todo->data << endl;
    todo = todo->next;
    ++count;
  }
}