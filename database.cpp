#include <postgresql/libpq-fe.h>
#include <iostream>
#include <string>
#include <string.h>
#include "database.hpp"

DataBase::DataBase(std::string db)
{
    /*подключается к базе данных от указанного имени пользователя;
    если базы данных с таким именем не существует, то она будет создана от имени postgres*/
    std::string conn_info = "dbname = " + db;
    m_conn = PQconnectdb(conn_info.c_str());
    ConnStatusType status = PQstatus(m_conn);
    std::string query = "CREATE DATABASE " + db;
    if (status == CONNECTION_BAD)
    {
        m_conn = PQconnectdb("dbname = postgres");
        PGresult * creating_status = PQexec(m_conn, query.c_str());
        PQclear(creating_status);
        std::cout << PQresultErrorMessage(creating_status);
        PQfinish(m_conn);
        m_conn = PQconnectdb(conn_info.c_str());
    }
}

DataBase::~DataBase()
{
    /*завершает соединение с базой данных*/
    PQfinish(m_conn);
}

void DataBase::add_table(std::string name, std::string id_name, std::string id_type)
{
    /*создает новую таблицу с единственным столбцом - идентификатором*/
    std::string query = "CREATE TABLE " + name + "(" + id_name + " " + id_type + " PRIMARY KEY" + ");"; 
    PGresult * adding_status = PQexec(m_conn, query.c_str());
    std::cout << PQresultErrorMessage(adding_status);
    PQclear(adding_status);
}

void DataBase::add_column(std::string table, std::string column, std::string type, std::string ref_table, std::string ref_column)
{
    /*добавляет столбцы к уже существующей таблице*/
    std::string query = "ALTER TABLE IF EXISTS " + table + " ADD COLUMN " + column + " " + type;
    PGresult * adding_status = PQexec(m_conn, query.c_str());
    std::cout << PQresultErrorMessage(adding_status);
    PQclear(adding_status);
    if (!ref_table.empty() && !ref_column.empty())
    {
        query = "ALTER TABLE IF EXISTS " + table + " ADD CONSTRAINT " + column + " FOREIGN KEY (" + column + ") REFERENCES " + ref_table 
        + "(" + ref_column + ") ON UPDATE CASCADE;";
        PGresult * adding_status = PQexec(m_conn, query.c_str());
        std::cout << PQresultErrorMessage(adding_status);
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
    PGresult * adding_status = PQexec(m_conn, query.c_str());
    std::cout << PQresultErrorMessage(adding_status);
    PQclear(adding_status);

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
    PGresult * editing_status = PQexec(m_conn, query.c_str());
    std::cout << PQresultErrorMessage(editing_status);
    PQclear(editing_status);
    return 0;
}

std::vector<std::vector<std::string>> DataBase::get_data(std::string table, const std::vector<std::string>& columns, std::string condition)
{
    /*возвращает все записи таблицы, или записи только из определенных столбцов*/ 
    std::string query = "SELECT ";
    if (columns.empty())
    {
        query += "* FROM " + table;
    }
    else if (columns.size() > 1)
    {
        for (int i = 0; i < columns.size() - 1; ++i)
        {
            query += columns[i] + ", ";
        }
        query += columns[columns.size()-1] + " FROM " + table;
    }
    else
    {
        query += columns[0] + " FROM " + table;
    }
    if (!condition.empty())
    {
        query += " WHERE " + condition;
    }
    query += ";";
    PGresult * getting_status = PQexec(m_conn, query.c_str());
    std::cout << PQresultErrorMessage(getting_status);
    int nstr = PQntuples(getting_status);
    int ncol = PQnfields(getting_status);
    std::vector<std::vector<std::string>> result;
    std::vector <std::string> str;
    for (int i = 0; i < nstr; ++i)
    {
        for (int j = 0; j < ncol; ++j)
        {
            str.push_back(PQgetvalue(getting_status, i, j));
        }
        result.push_back(str);
        str.clear();
    }
    PQclear(getting_status);
    return result;
}