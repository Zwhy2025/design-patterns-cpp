#include <iostream>
#include <list>
#include <mutex>

class Observer;

class Subject
{
public:
    std::list<Observer *> _listObserver;
    std::mutex _mutex_list;
public:
    virtual void attach(Observer *o) = 0;
    virtual void detach(Observer *o) = 0;
    virtual void notify() = 0;
};

class Stock : public Subject
{
public:
    int _nPrice = 0;
    std::mutex _mutex_p;
public:
    int getPrice();
    void setPrice(int);
    void attach(Observer *o) override;
    void detach(Observer *o) override;
    void notify() override;
};

class Observer
{
public:
    Subject *_tSub;
public:
    Observer(Subject *sub);
    virtual ~Observer();
    virtual void update() = 0;
};

Observer::Observer(Subject* sub): _tSub(sub){
    _tSub->attach(this);
}
Observer::~Observer(){
    _tSub->detach(this);
}
class Monitor : public Observer
{
public:
    Monitor(Subject *sub) : Observer(sub) {}
    void print(int v) const
    {
        std::cout << "Monitor: " << v << std::endl;
    }
    
    void update() override{
        print(static_cast<Stock*>(_tSub)->getPrice());
    }
};

class Billboard : public Observer
{
public:
    Billboard(Stock* stock) : Observer(stock) {}
    void display(int v) const
    {
        std::cout << "Billboard: " << v << std::endl;
    }

    void update() override{
        display(static_cast<Stock*>(_tSub)->getPrice());
    }
};

int Stock::getPrice(){
    std::unique_lock<std::mutex> lock(_mutex_p);
    return _nPrice;
}

void Stock::setPrice(int v){
    std::unique_lock<std::mutex> lock(_mutex_p);
    _nPrice = v;
    this->notify();
}

void Stock::attach(Observer* o){
    std::unique_lock<std::mutex> lock(_mutex_list);
    _listObserver.push_back(o);
}

void Stock::detach(Observer* o){
    std::unique_lock<std::mutex> lock(_mutex_list);
    _listObserver.remove(o);
}

void Stock::notify(){
    std::unique_lock<std::mutex> lock(_mutex_list);
    for(auto observer : _listObserver){
        observer->update();
    }
}

int main(){
    Stock stock;
    Monitor monitor {&stock};
    Billboard billboard {&stock};
    stock.setPrice(10);
}
