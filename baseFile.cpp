#include <iostream>
#include <vector>
#include <exception>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Product
{
public:
    string name;
    double price;

    Product() : name(""), price(0.0) {}
    Product(string name, double price) : name(name), price(price) {}

    friend ostream &operator<<(ostream &os, const Product &product)
    {
        os << "Name: " << product.name << ", Price: " << product.price;
        return os;
    }

    friend istream &operator>>(istream &is, Product &product)
    {
        is >> product.name >> product.price;
        return is;
    }
};

class FileManager
{
private:
    string filename;

public:
    FileManager(string filename) : filename(filename) {}

    void saveProducts(const vector<Product> &products, const string& filename) {
  // Open the file for reading and writing (ios::in | ios::out)
  fstream file(filename, ios::in | ios::out);

  if (!file.is_open()) {
    cerr << "Failed to open file for reading and writing." << endl;
    return;
  }

  string line, name;
  double price;
  bool replaced = false; // Flag to track if a replacement occurred

  // Read existing file content line by line
  while (getline(file, line)) {
    // Extract product name and price (assuming comma separation)
    stringstream ss(line);
    getline(ss, name, ',');
    ss >> price;

    // Check if product name already exists
    for (const auto& product : products) {
      if (product.name == name) {
        // Replace existing product
        file.seekp(file.tellg() - static_cast<streampos>(line.length()) - 1); // Move back to the beginning of the current line
        file << product.name << "," << product.price << "\n";
        replaced = true;
        break; // Exit inner loop after finding and replacing
      }
    }
  }

  // If no replacements were made, write new products to end of file
  if (!replaced) {
    file.seekp(ios::end); // Move to the end of the file
    for (const auto& product : products) {
      file << product.name << "," << product.price << "\n";
    }
  }

  file.close();
}

    vector<Product> loadProducts()
    {
        std::ifstream file(filename);
        std::vector<Product> products;

        if (!file.is_open())
        {
            std::cerr << "Failed to open file for reading." << std::endl;
            return products;
        }

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string name;
            double price;
            char delimiter; // Declare a char variable for the delimiter
            if (!(iss >> name >> delimiter >> price))
            { // Extract the delimiter as a char
                std::cerr << "Invalid line format." << std::endl;
                continue;
            }
            // Compare the delimiter to a single space character, not a string literal
            if (delimiter != ' ')
            {
                std::cerr << "Invalid line format." << std::endl;
                continue;
            }
            products.push_back(Product(name, price));
        }

        file.close();
        return products;
    }
};

class SagaStep
{
public:
    virtual void execute() = 0;
    virtual void compensate() = 0;
    virtual ~SagaStep() {}
};

class PaymentSagaStep : public SagaStep
{
public:
    void execute() override
    {
        std::cout << "Executing payment." << std::endl;
        // Add payment logic here
    }

    void compensate() override
    {
        std::cout << "Compensating payment." << std::endl;
        // Add payment compensation logic here
    }
};

class OrderSagaStep : public SagaStep
{
public:
    void execute() override
    {
        std::cout << "Placing order." << std::endl;
    }
    void compensate() override
    {
        std::cout << "Compensating order." << std::endl;
    }
};

class InventorySagaStep : public SagaStep
{
public:
    void execute() override
    {
        std::cout << "Executing inventory." << std::endl;
        // Add inventory logic here
    }

    void compensate() override
    {
        std::cout << "Compensating inventory." << std::endl;
        // Add inventory compensation logic here
    }
};

class DeliverySagaStep : public SagaStep
{
public:
    void execute() override
    {
        std::cout << "Executing delivery." << std::endl;
    }
    void compensate() override
    {
        std::cout << "Compensating delivery." << std::endl;
    }
};

class Saga
{
public:
    void addStep(SagaStep *step)
    {
        steps.push_back(step);
    }

    void execute()
    {
        try
        {
            for (auto &step : steps)
            {
                step->execute();
                completedSteps.push_back(step);
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "Error: " << e.what() << ". Compensating..." << std::endl;
            compensate();
            throw;
        }
    }

private:
    void compensate()
    {
        for (auto it = completedSteps.rbegin(); it != completedSteps.rend(); ++it)
        {
            (*it)->compensate();
        }
    }

    std::vector<SagaStep *> steps;
    std::vector<SagaStep *> completedSteps;
};

class SagaCoordinator
{
public:
    explicit SagaCoordinator(Saga *saga) : saga(saga) {}

    void executeSaga()
    {
        try
        {
            saga->execute();
            std::cout << "Saga executed successfully." << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "Saga execution failed: " << e.what() << std::endl;
        }
    }

private:
    Saga *saga;
};

int main()
{
    vector<Product> products;
    FileManager fileManager("products.txt");

    int choice;
    do
    {
        cout << "Menu:\n";
        cout << "1. Add product\n";
        cout << "2. Load products\n";
        cout << "3. Display products\n";
        cout << "4. Quit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string productName;
            double productPrice;
            cout << "Enter product name: ";
            cin.ignore(); // Ignore newline character from previous input
            getline(cin, productName);
            cout << "Enter product price: ";
            cin >> productPrice;
            products.push_back(Product(productName, productPrice));

            char saveChoice;
            cout << "Save this product now? (y/n): ";
            cin >> saveChoice;
            if (saveChoice == 'y' || saveChoice == 'Y')
            {
                fileManager.saveProducts(products);
                cout << "Product saved.\n";
            }
            break;
        }

        case 2:
        {
            products = fileManager.loadProducts();
            std::cout << "Products loaded.\n";
            break;
        }
        case 3:
        {
            for (const auto &product : products)
            {
                std::cout << product << std::endl;
            }
            break;
        }
        case 4:
        {
            std::cout << "Exiting...\n";
            break;
        }
        default:
        {
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
        }
    } while (choice != 4);

    return 0;
}
