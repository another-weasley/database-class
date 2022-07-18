#ifndef DATABASE_H
#define DATABASE_H
#include <postgresql/libpq-fe.h>
#include <string>
#include <vector>
class DataBase
{   
    PGconn * m_conn; 
    public:
        DataBase(std::string db);
        ~DataBase();
        void add_table(std::string name, std::string id_name = "id", std::string id_type = "integer");
        void add_column(std::string table, std::string column, std::string type, std::string ref_table = "", std::string ref_column = "");
        int add_item(std::string table, const std::vector <std::string>& columns, const std::vector <std::string>& values);
        int edit_item(std::string table, std::string condition, const std::vector <std::string>& columns, const std::vector <std::string>& values);
        std::vector<std::vector<std::string>> get_data(std::string table, const std::vector<std::string>& columns = {}, std::string condition = "");
};

#endif // DATABASE_H