#include <postgresql/libpq-fe.h>
#include <iostream>
#include <string>
#include <string.h>
#include "database.hpp"

DataBase::DataBase(const char * db)
{
    /*подключается к базе данных от указанного имени пользователя;
    если базы данных с таким именем не существует, то она будет создана от имени postgres*/
    char conn_info[] = "dbname = ";
    strcat(conn_info, db);
    m_conn = PQconnectdb(conn_info);
    ConnStatusType status = PQstatus(m_conn);
    char query[] = "CREATE DATABASE ";
    strcat(query, db);
    if (status == CONNECTION_BAD)
    {
        m_conn = PQconnectdb("dbname = postgres");
        PGresult * creating_status = PQexec(m_conn, query);
        PQclear(creating_status);
        PQfinish(m_conn);
        m_conn = PQconnectdb(conn_info);
        std::cout << "Создана новая база данных" << std::endl;
    }
    else
    {
        std::cout << "Соедиение с базой данных установлено" << std::endl;
    }
}

DataBase::~DataBase()
{
    /*завершает соединение с базой данных*/
    PQfinish(m_conn);
    std::cout << "Соединение с базой данных закрыто" << std::endl;
}

void DataBase::add_table(std::string name, std::string id_name, std::string id_type)
{
    /*создает новую таблицу с единственным столбцом - идентификатором*/
    std::string query = "CREATE TABLE " + name + "(" + id_name + " " + id_type + " PRIMARY KEY" + ");"; 
    PGresult * adding_status = PQexec(m_conn, query.c_str());
    PQclear(adding_status);
}

void DataBase::add_column(std::string table, std::string column, std::string type, std::string ref_table, std::string ref_column)
{
    /*добавляет столбцы к уже существующей таблице*/
    std::string query = "ALTER TABLE IF EXISTS " + table + " ADD COLUMN " + column + " " + type;
    PGresult * adding_status = PQexec(m_conn, query.c_str());
    PQclear(adding_status);
    if (!ref_table.empty() && !ref_column.empty())
    {
        query = "ALTER TABLE IF EXISTS " + table + " ADD CONSTRAINT " + column + " FOREIGN KEY (" + column + ") REFERENCES " + ref_table + "(" + ref_column + ");";
        std::cout << query << std::endl; 
        PGresult * adding_status = PQexec(m_conn, query.c_str());
        PQclear(adding_status);
    }
}

int DataBase::add_item(std::string table, const std::vector <std::string>& columns, const std::vector <std::string>& values)
{
    /*добавляет новую запись в указанную таблицу*/
    /*строковые данные должны быть заключены в одинарные кавычки*/
    std::string query = "INSERT INTO " + table + " ";
    std::string columns_query = "(";
    std::string values_query = "(";
    if (columns.size() != values.size())
    {
        std::cout << "Ошибка в вызове DataBase.add_item(): columns и values должны иметь одинаковый размер." << std::endl;
        return 1;
    }
    if (columns.size() > 1)
    {
        for (int i = 0; i < columns.size() - 1; ++i)
        {
            columns_query += columns[i] + ",";
            values_query += values[i] + ",";
        }
        columns_query += columns[columns.size() - 1] + ")";
        values_query += values[values.size() - 1] + ")";
    }
    else
    {
        columns_query += columns[0] + ")";
        values_query += values[0] + ")";
    }
    query += columns_query + " VALUES " + values_query + ";";
    std::cout << query;
    PGresult * adding_status = PQexec(m_conn, query.c_str());
    PQclear(adding_status);
    std::cout << "item was added" << std::endl;

    return 0;
}

int DataBase::edit_item(std::string table, std::string condition, const std::vector <std::string>& columns, const std::vector <std::string>& values)
{
    std::string query = "UPDATE " + table + " SET ";
    if (columns.size() != values.size())
    {
        std::cout << "Ошибка в вызове DataBase.edit_item(): columns и values должны иметь одинаковый размер." << std::endl;
        return 1;
    }
    if (columns.size() > 1)
    {
        for (int i = 0; i < columns.size() - 1; ++i)
        {
            query += columns[i] + " = " + values[i] + ", ";
        }
        query += columns[columns.size()-1] + " = " + values[values.size()-1] + " WHERE " + condition + ";";
    }
    else
    {
        query += columns[0] + " = " + values[0] + " WHERE " + condition + ";";
    }
    std::cout << query << std::endl;
    PGresult * editing_status = PQexec(m_conn, query.c_str());
    PQclear(editing_status);
    std::cout << "edited" << std::endl;
    return 0;
}

std::vector<std::vector<std::string>>& DataBase::get_data(std::string table, const std::vector<std::string>& columns = {})
{
    /*возвращает все записи таблицы, или записи только из определенных столбцов*/ 
    std::string query = "SELECT ";
    if (columns.empty())
    {
        query += "* FROM " + table + ";";
    }
    else if (columns.size() > 1)
    {
        for (int i = 0; i < columns.size() - 1; ++i)
        {
            query += columns[i] + ", ";
        }
        query += columns[columns.size()-1] + " FROM " + table + ";";
    }
    else
    {
        query += columns[0] + " FROM " + table + ";";
    }

    std::cout << query << std::endl;
    PGresult * getting_status = PQexec(m_conn, query.c_str());
    int nstr = PQntuples(getting_status);
    int ncol = PQnfields(getting_status);
    PQclear(getting_status);
}