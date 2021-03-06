import system'collections.
import system'routines.
import extensions.
import extensions'routines.
import extensions'text.

class Employee
{
    object theName.
    object theID.
    object theSalary.
    object theDepartment.
    
    constructor new name:name id:id salary:salary department:department
    [
        theName := name.
        theID := id.
        theSalary := salary.
        theDepartment := department.
    ]
    
    Name = theName.
    
    Salary = theSalary.
    
    Department = theDepartment.
    
    literal
        = StringWriter new;
            write:theName paddingRight:25;
            write:theID paddingRight:12;
            write(theSalary literal) paddingRight:12;
            write:theDepartment.
}
                                
extension reportOp
{
    topNPerDepartment:n
        = self groupBy(:x)( x Department ); selectBy(:x)
        [
            ^ {
                Department = x key.
                
                Employees
                    = x orderBy(:f:l)( f Salary > l Salary ); top:n; summarize(ArrayList new).
            }.
        ].
}

program =
[
    var employees :=
    (
        Employee new name:"Tyler Bennett" id:"E10297" salary:32000 department:"D101",
        Employee new name:"John Rappl" id:"E21437" salary:47000 department:"D050",
        Employee new name:"George Woltman" id:"E00127" salary:53500 department:"D101",
        Employee new name:"Adam Smith" id:"E63535" salary:18000 department:"D202",
        Employee new name:"Claire Buckman" id:"E39876" salary:27800 department:"D202",
        Employee new name:"David McClellan" id:"E04242" salary:41500 department:"D101",
        Employee new name:"Rich Holcomb" id:"E01234" salary:49500 department:"D202",
        Employee new name:"Nathan Adams" id:"E41298" salary:21900 department:"D050",
        Employee new name:"Richard Potter" id:"E43128" salary:15900 department:"D101",
        Employee new name:"David Motsinger" id:"E27002" salary:19250 department:"D202",
        Employee new name:"Tim Sampair" id:"E03033" salary:27000 department:"D101",
        Employee new name:"Kim Arlich" id:"E10001" salary:57000 department:"D190",
        Employee new name:"Timothy Grove" id:"E16398" salary:29900 department:"D190"
    ).
    
    employees topNPerDepartment:2; forEach(:info)
    [
        console printLine("Department: ",info Department).
        
        info Employees; forEach:printingLn.
        
        console writeLine:"---------------------------------------------"
    ].
    
    console readChar
].