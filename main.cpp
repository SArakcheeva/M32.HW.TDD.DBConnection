/* Задание: создать тесты с моком объекта, имитирующим подключение к базе данных.

   Напишите реализацию классов (которые будут участвовать в тестах), как показано на схеме:
   
        ClassThatUsesDB:                      DBConnection
        openConnection()       <-----           open()
        useConnection()                         close()
        closeConnection()                       execQuery()
                |
                |
              Tests
    
    Напишите тесты с использованием методологии TDD, фреймворков GTest и GMock.
 */ 

#include "gtest/gtest.h"
#include "gmock/gmock.h"

//класс гипотетических команд
class SomeConnectDBCommand
{
};

//класс - дескриптор соединения с базой данных
class Connection
{
public:
    Connection()
    {}

    //добавляем необходимые нам методы
    Connection(const char* data)
    {}

    //добавляем необходимые нам методы
    bool operator==(const Connection& other) const
    {
        return true;
    }
};


//класс ответа
class ClassResult
{
public:
    //добавляем необходимые нам методы   
    bool operator==(const ClassResult) const
    {
        return true;
    }
};
 

// теперь переходим к использованию интерфейсов
// это хорошо и с архитектурной точки зрения, и с точки зрения дальнейшего использования мок-объектов
class DBConnectionInterface
{
public:
    DBConnectionInterface()
    {}

    virtual ~DBConnectionInterface()
    {}

    //метод открытия соединение с базой данных
    virtual Connection open(const SomeConnectDBCommand& cmd) = 0;

    //метод закрытия соединение с сервером базы данных
    virtual bool close(const Connection& conn) = 0;

    //метод - результат запроса к базе данных
    virtual ClassResult execQuery(const Connection& conn, const SomeConnectDBCommand& cmd) = 0;
};


//теперь реализуем наш класс DBConnection на основе интерфейса
class DBConnection : public DBConnectionInterface
{
public:
    DBConnection()
    {}

    virtual ~DBConnection()
    {}

    virtual Connection open(const SomeConnectDBCommand& cmd) override
    {
        return Connection();
    }

    virtual bool close(const Connection& conn) override
    {
        return true;
    }

    virtual ClassResult execQuery(const Connection& conn, const SomeConnectDBCommand& cmd) override
    {
        return ClassResult();
    }
};


// класс, иммитирующий подключение к базе данных
// в использовании класса тоже переходим на интерфейсы
// также не стоит забывать о том, что у нас может быть несколько реализаций или версий протокола
class ClassThatUsesDBInterface
{
public:
    ClassThatUsesDBInterface()
    {}

    virtual ~ClassThatUsesDBInterface()
    {}

    // Метод, выполняющий соединение с базой данных
    // Функция возвращает дескриптор соединения, переданный в первом параметре, 
    // если соединение было успешным или NULL если соединение было неудачным
    virtual Connection openConnection(const SomeConnectDBCommand& cmd) const = 0;

    //метод, который закрывает соединение с сервером базы данных
    virtual bool closeConnection(const Connection& conn) const = 0;

    // Метод, выполняющий сложный обмен данными по каналу
    virtual ClassResult useConnection(DBConnectionInterface* chanel, const SomeConnectDBCommand& cmd) = 0;
};


//реализация конкретного протокола на основе интерфейса
//так же, как и в прошлом случае, все методы пока пустые
class ClassThatUsesDB : public ClassThatUsesDBInterface
{
public:
    ClassThatUsesDB()
    {}

    virtual ~ClassThatUsesDB()
    {}

    // Метод, выполняющий соединение с базой данных
    virtual Connection openConnection(const SomeConnectDBCommand& cmd) const override
    {
        return Connection();
    }
    
    // метод, преобразующий полученные данные в ответ
    virtual bool closeConnection(const Connection& conn) const override
    {
        return true;
    }

    virtual ClassResult useConnection(DBConnectionInterface* chanel, const SomeConnectDBCommand& cmd)
    {
        Connection conn = chanel->open(cmd);

        ClassResult responceData = chanel->execQuery(conn, cmd);
        
        return ClassResult();
    } 
};


//создаем класс мок-объекта соединения с базой данных
class MockExchangeDBConnection : public DBConnectionInterface
{
public:
    MOCK_METHOD(Connection, open, (const SomeConnectDBCommand& cmd), (override));  
    MOCK_METHOD(bool, close, (const Connection& conn), (override));
    MOCK_METHOD(ClassResult, execQuery, (const Connection& conn, const SomeConnectDBCommand& cmd), (override));
};


//создаем фикстуру и тирдаун для наших тестов
class SomeTestSuite : public ::testing::Test
{
protected:
    void SetUp()
    {
        someConnection = new ClassThatUsesDB();
    }

    void TearDown()
    {
        delete someConnection;
    }

protected:
    ClassThatUsesDB* someConnection;
};


TEST_F(SomeTestSuite, testcase1)
{
    //тестируем преобразование некой команды в дескриптор содинения с базой данных
    SomeConnectDBCommand cmd;

    //преобразуем команду в дескриптор содинения с базой данных в соответствии с протоколом
    Connection conn = someConnection->openConnection(cmd);

    //референс данных, которые должны быть получены
    Connection reference("MYSQL_CONN");

    //сравниваем полученное значение с референсом
    ASSERT_EQ(conn, reference);
}


TEST_F(SomeTestSuite, testcase2)
{
    //тестируем разбор возвращаемых данных в соответствии с протоколом 
    //и превращение их в объект-ответ
    Connection conn;

    //тестовые данные — гипотетический ответ
    bool reference = true;

    bool responce = someConnection->closeConnection(conn);

    //сравниваем полученное значение с референсом
    ASSERT_EQ(reference, responce);
}


// тест на обмен с использованием мок-объектов
TEST_F(SomeTestSuite, testcase3)
{
    // данные, которые мы планируем использовать как данные и ответы мок-объекта
    SomeConnectDBCommand cmd;
    Connection conn;
    ClassResult result;

    // создаем мок-объект
    MockExchangeDBConnection mconnect;

    // прописываем «ожидания»
    // надеемся, что будет хотя бы раз вызван метод open()
    // если он будет вызван, то вернет значение conn
    EXPECT_CALL(mconnect, open).WillOnce(::testing::Return(conn));

    // надеемся, что будет вызван метод close(), возможно, несколько раз, и каждый раз будет возвращать true
    EXPECT_CALL(mconnect, close).WillRepeatedly(::testing::Return(true));

    // ожидаем, что будет вызван метод execQuery
    // если это произойдет, то он вернет result
    EXPECT_CALL(mconnect, execQuery).WillOnce(::testing::Return(result));


    // запускаем алгоритм на обработку
    ClassResult responce = someConnection->useConnection(&mconnect, cmd);

    // сравниваем полученный результат с референсом
    ASSERT_EQ(result, responce);
}


int main(int argc, char* argv[])
{
    // Инициализация и запуск тестов
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}