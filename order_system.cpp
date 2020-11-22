#include "order_system.hpp"
#include "httplib.h"

_order_sys::DishTable* dish_table;
_order_sys::OrderTable* order_table;

void InsertDish(const httplib::Request& req, httplib::Response& rsp)
{
	Json::Value dish_val;
	
	Json::Reader reader;
	bool ret = reader.parse(req.body, dish_val);
	if(ret == false)
	{
		std::cout << "InsertDish parse json data failed!!\n";
		rsp.status = 400;
	}
	dish_table->Insert(dish_val);
	if(ret ==false)
	{
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;
}
void DeleteDish(const httplib::Request& req, httplib::Response& rsp)
{
	// path = "/dish/100"; /dish/(\d+) 
	// 匹配 /dish/数字 这种格式的字符串
	int dish_id = std::stoi(req.matches[1]);

	bool ret = dish_table->Delete(dish_id);
	if(ret == false)
	{
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;
}
void UpdateDish(const httplib::Request& req, httplib::Response& rsp)
{
	Json::Value dish_val;
	Json::Reader reader;
	bool ret = reader.parse(req.body, dish_val);
	if(ret ==false)
	{
		rsp.status == 200;
		return ;
	}
	ret = dish_table->Update(dish_val);
	if(ret == false)
	{
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;
}
void SelectOneDish(const httplib::Request& req, httplib::Response& rsp)
{
	int dish_id = std::stoi(req.matches[1]);
	Json::Value dish_val;
	bool ret = dish_table->SelectOne(dish_id, &dish_val);
	if(ret == false)
	{
		rsp.status = 500;
		return ;
	}
	Json::FastWriter writer;
	std::string body = writer.write(dish_val);
	
	rsp.set_content(body.c_str(), body.size(), "application/json");
	rsp.status = 200;
	return ;
}
void SelectAllDish(const httplib::Request& req, httplib::Response& rsp)
{
	Json::Value dishes_val;
	bool ret = dish_table->SelectAll(&dishes_val);
	if(ret == false)
	{
		rsp.status = 500;
		return ;
	}
	std::string body;
	Json::FastWriter writer;
	body = writer.write(dishes_val);

	rsp.set_content(body.c_str(), body.size(), "application/json");
	rsp.status = 200;
	return ;
}
void InsertOrder(const httplib::Request& req, httplib::Response& rsp)
{
	Json::Value order_val;
	
	Json::Reader reader;
	bool ret = reader.parse(req.body, order_val);
	if(ret == false)
	{
		std::cout << "InsertOrder parse json failed!!\n";
		rsp.status = 400;
		return ;
	}
	ret = order_table->Insert(order_val);
	if(ret ==false)
	{
		std::cout << "InsertOrder insert data to db failed!\n";
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;
}
void DeleteOrder(const httplib::Request& req, httplib::Response& rsp)
{
	int order_id = std::stoi(req.matches[1]);

	bool ret = order_table->Delete(order_id);
	if(ret == false)
	{
		std::cout << "DeleteOrder delete order failed!\n";
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;
}
void UpdateOrder(const httplib::Request& req, httplib::Response& rsp)
{
	Json::Value order_val;
	Json::Reader reader;

	bool ret = reader.parse(req.body, order_val);
	if(ret ==false)
	{
		std::cout << "UpdateOrder parse json failed!\n";
		rsp.status = 400;
		return ;
	}
	ret = order_table->Update(order_val);
	if(ret == false)
	{
		std::cout << "UpdateOrder update data to db failed\n";
		return ;
	}
	rsp.status = 200;
	return ;
}
void SelectOneOrder(const httplib::Request& req, httplib::Response& rsp)
{
	int order_id = std::stoi(req.matches[1]);
	Json::Value order_val;
	bool ret = order_table->SelectOne(order_id, &order_val);
	if(ret == false)
	{
		std::cout << "SelectOneOrder get order info from db failed\n";
		return ;
	}

	std::string body;
	Json::FastWriter writer;
        body = writer.write(order_val);
	
	rsp.set_content(body.c_str(), body.size(), "application/json");
	rsp.status = 200;
	return ;
}
void SelectAllOrder(const httplib::Request& req, httplib::Response& rsp)
{
	Json::Value order_val;
	bool ret = order_table->SelectAll(&order_val);
	if(ret == false)
	{
		std::cout << "SelectAllOrder get order info from db failed\n";
		return ;
	}

	std::string body;
	Json::FastWriter writer;
	body = writer.write(order_val);

	rsp.set_content(body.c_str(), body.size(), "application/json");
	rsp.status = 200;
	return ;
}

#define WWWROOT "./wwwroot"	
int main()
{
	MYSQL* mysql = _order_sys::MysqlInit();
	dish_table = new _order_sys::DishTable(mysql);
	order_table = new _order_sys::OrderTable(mysql);

	httplib::Server server;
	//插入菜品请求 POST	
	server.Post("/dish", InsertDish);
	//删除菜品请求 DELETE
	server.Delete(R"(/dish/(\d+))", DeleteDish);
	//更新菜品请求 PUT
	server.Put("/dish", UpdateDish);
	//查询单个菜品请求 GET/dish/100
	server.Get(R"(/dish/(\d+))", SelectOneDish);
	//查询所有菜品请求 GET
	server.Get("/dish", SelectAllDish);
	//订信息的增删改查
	server.Post("/order", InsertOrder);
	server.Delete(R"(/order/(\d+))", DeleteOrder);
	server.Put("/order", UpdateOrder);
	server.Get(R"(/order/(\d+))", SelectOneOrder);
	server.Get("/order", SelectAllOrder);

	server.set_base_dir(WWWROOT);	
	server.listen("0.0.0.0", 9000);
	_order_sys::MysqlDestroy(mysql);	
	return 0;
}
