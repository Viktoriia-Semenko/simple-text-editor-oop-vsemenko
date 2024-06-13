# Simple Text Editor: the basics of object-oriented programming
## Purpose of work
Understand the basics of object-oriented programming. Get skills in
rewriting procedural/structural code into the object-oriented one. Understand and define the
benefits of using object-oriented paradigm comparing to procedural/structural paradigms.

## Task
Rewrite the existing code using object-oriented paradigm. </br>
Upgrade a console-based text editor with below functions:
1. Delete command
2. Undo command (3 steps, ignoring Save/Load commands)
3. Redo command (3 steps, ignoring Save/Load commands)
4. Cut/Copy/Paste commands
5. Insert with replacement command
6. (Optional) Implement cursor-based logic </br>
In this case the logic for cut, copy, paste, delete, insert and some other commands will be
based on the current cursor location.

## Control questions
1. What is object-oriented paradigm?
   - це парадигма, у якій програма написана як набір класів. Кожен клас має екземпляри, які називаються об’єктами
    <br></br>
2. What is a class?
   - це шаблон або схема для створення об'єктів. Він визначає дані та методи, які будуть спільними для всіх об'єктів цього класу.
   - Формально клас розглядається як набір даних, таких як поля, атрибути, члени класу та функції, тобто методи для роботи з ними.
<br></br>
3. What is an object?
   - це конкретний екземпляр класу, який має певні значення полів та може виконувати методи класу.
<br></br>
4. What is the difference between fields and methods?
   - Поля (або властивості) - це змінні, що зберігають дані про стан об'єкта.
   - Методи - це функції, що визначають поведінку об'єкта, тобто те, що об'єкт може робити.
<br></br>    
5. Enumerate key object-oriented principles.
   - Encapsulation - “чутливі” дані приховані від користувача
   - Inheritance - успадкування атрибутів і методів від base(parent) class to derived(child)
   - Polymorphism - “багато форм” - класів створених успадкуванням
   - Abstraction - узагальнені класи
<br></br>
6. Explain benefits of using object-oriented paradigm comparing to procedural and structural.
   - код зрозумілішим стає
   - схожим на обʼєкти реального світу
   - читання коду краще стає
   - вищий рівень абстракціїї
   - краще керування лайф тайм обʼєктів
   - по менеджменту памʼяті краще 
   - інкапсуляція: в один об’єкт або клас об’єднуються і дані, і методи, які працюють з цими даними. Це приховування внутрішньої структури об’єкта від зовнішніх впливів. Всі зміни стану об’єкта відбуваються тільки за допомогою звернень до методів самого об’єкту.
<br></br>
7. Describe the program translation process from the source code to the final executable.
   <img width="616" alt="Знімок екрана 2024-06-11 о 23 06 54" src="https://github.com/Viktoriia-Semenko/simple-text-editor-oop-vsemenko/assets/150340301/5ec1a62d-2d6f-4fc1-ba7e-391f1571958c">
   - Compiler - програма, написана людьми, дозволяє перетворити файл у набір байтів (асемблерний код), який буде зрозумілий на 
центральному процесорі
     - якщо **compile error** - перевіряємо наскільки написаний нами код відповідає стандарту мові програмування
   - main.o/main.obj - перетворити файл main.cpp у інший вид
     - містить набір асемблерних інструкцій
   - Linker - ще одна програма, яка поєднує всі наші файли та зовнішні бібліотеки, треба поєднати все у єдине ціле
   - Executable file - файл який конкретна операційна система буде знати як запустити
<br></br>
8. What is compiler responsible for?
   - Compiler - програма, написана людьми, дозволяє перетворити файл у набір байтів (асемблерний код), який буде зрозумілий на 
центральному процесорі
   - Відповідає за переклад початкового коду, написаного мовою програмування високого рівня, в машинний код, який може виконуватись комп'ютером.
<br></br>
9. What is a linker?
    - ще одна програма, яка поєднує всі наші файли та зовнішні бібліотеки, треба поєднати все у єдине ціле
    - це інструмент, що об'єднує різні об'єктні файли та бібліотеки в єдиний виконуваний файл. Він забезпечує правильну адресацію та розташування всіх частин програми в пам'яті.

## Test cases
[Test cases for Assignment 2](https://github.com/Viktoriia-Semenko/simple-text-editor-oop-vsemenko/blob/main/test_cases.md)
