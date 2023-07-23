# M32.HW.TDD.DBConnection
О программе

• Программа представляет создание тестов с моком-объектом, имитирующим подключение к базе данных.

• Реализацию классов (которые будут участвовать в тестах), реализованы согласно схеме:
   
        ClassThatUsesDB:                      DBConnection
        openConnection()       <-----           open()
        useConnection()                         close()
        closeConnection()                       execQuery()
                |
                |
              Tests
    
 • Тесты написаны с использованием методологии TDD, фреймворков GTest и GMock.
