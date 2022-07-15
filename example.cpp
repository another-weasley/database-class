#include "database.hpp"

int main()
{
    DataBase sample("test");
    sample.add_table("first");
    
    sample.add_table("second");
    sample.add_column("second", "test1", "float");
    sample.add_column("second", "test2", "text");
    sample.add_item("second", {"test2", "test1", "id"}, {"'hello'", "4.99", "1"});
    sample.add_column("second", "fconn_id", "integer", "first", "id");
    sample.add_item("second", {"id", "fconn_id"}, {"3", "1"});
    sample.edit_item("second", "id = 1", {"test1"}, {"4.02"});
    sample.edit_item("second", "id = 2", {"test1", "test2"}, {"10.10", "'wow'"});
    return 0;
}