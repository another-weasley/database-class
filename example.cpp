#include <iostream>
#include "database.hpp"

void print_vector(const std::vector<std::vector<std::string>>& vec)
{
    for (auto str: vec)
    {
        for (auto item: str)
        {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    // создание бд
    DataBase sample("cats_order");
    // создание таблиц в бд
    sample.add_table("customers", "passport");
    sample.add_table("orders");
    sample.add_table("products");
    // добавление колонок в таблицы
    sample.add_column("customers", "name", "text");
    sample.add_column("orders", "created", "date");
    sample.add_column("products", "product_name", "text");
    // добавление ссылок на другие таблицы
    sample.add_column("orders", "customer_id", "integer", "customers", "passport");
    sample.add_column("products", "order_id", "integer", "orders", "id");
    // добавление записей в таблицы
    sample.add_item("customers", {"passport", "name"}, {"0000123456", "'Мисочкин Котий Мурзикович'"});
    sample.add_item("customers", {"name", "passport"}, {"'Пушистая Мурка Барсиковна'", "1111234567"});
    sample.add_item("orders", {"id", "created", "customer_id"}, {"1234", "'2021-10-23'", "0000123456"});
    sample.add_item("products", {"id", "product_name", "order_id"}, {"100", "'Pro Plan Delicate'", "1234"});
    sample.add_item("products", {"id", "product_name", "order_id"}, {"110", "'мышь-пищалка'", "1234"});
    // изменение записей
    sample.edit_item("orders", "id = 1234", {"customer_id"}, {"1111234567"});
    sample.edit_item("products", "id = 00010", {"product_name"}, {"'мышь-сопелка'"});
    // получение записей
    std::vector<std::vector<std::string>> products = sample.get_data("products", {"id", "product_name"});
    print_vector(products);
    return 0;
}