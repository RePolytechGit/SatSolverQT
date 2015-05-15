#include "PClasses.h"


Sat_Solver::Sat_Solver()
{
}

Sat_Solver::~Sat_Solver()
{
	delete[] function;
	delete[] variables;
	changes.clear();
}

bool Sat_Solver::Solution_search()
{
    int i = 0;
    while (i < function_size && function[i].checker)
		i++;
	if (i == function_size)
		return true;
	else
		return false;
}

int Sat_Solver::Check(clauses* object)
{
    int counter = 0;
    for (int i = 0; i < object->negliterals.size(); i++)
    if (!variables[object->negliterals[i]].value || !variables[object->negliterals[i]].determined)
	{
        counter++;
	}
    for (int i = 0; i < object->posliterals.size(); i++)
    if (variables[object->posliterals[i]].value || !variables[object->posliterals[i]].determined)
	{
        counter++;
	}
    return counter;
}

bool Sat_Solver::Next(clauses* object, variable*& next_var)
{
    object->checker = true;
	local_changes.push_back(object);
    for (int i = 0; i < object->negliterals.size(); i++)
    if (!variables[object->negliterals[i]].determined)
	{
        next_var = &variables[object->negliterals[i]];
        next_var->determined = true;
		next_var->value = false;
		return true;
	}
    for (int i = 0; i < object->posliterals.size(); i++)
    if (!variables[object->posliterals[i]].determined)
	{
        next_var = &variables[object->posliterals[i]];
        next_var->determined = true;
		next_var->value = true;
		return true;
	}
	return false;
}

void Sat_Solver::deinit_var(variable*& new_variable)
{
    new_variable->determined = false;
	new_variable = NULL;
}

bool Sat_Solver::Unit_propagation(variable* new_variable)
{
	if (new_variable == NULL)
		return true;
	if (new_variable->value)
	{
        for (int i = 0; i < new_variable->neglist.size(); i++)
        if (!new_variable->neglist[i]->checker)
		{
            unsigned short int a = Check(new_variable->neglist[i]);
			if (a == 0)
				return false;
			if (a == 1)
			{
                variable* next_variable = NULL;
                if (Next(new_variable->neglist[i], next_variable))
				{
					local_var_changes.push_back(next_variable);
                    Brackets_checking(next_variable);
                    if (!Unit_propagation(next_variable))
					{
						return false;
					}
				}
				else
					return false;
			}
		}
	}
	else
	{
        for (int i = 0; i < new_variable->poslist.size(); i++)
        if (!new_variable->poslist[i]->checker)
		{
            unsigned short int a = Check(new_variable->poslist[i]);
			if (a == 0)
				return false;
			if (a == 1)
			{
                variable* next_variable = NULL;
                if (Next(new_variable->poslist[i], next_variable))
				{
					local_var_changes.push_back(next_variable);
                    Brackets_checking(next_variable);
                    if (!Unit_propagation(next_variable))
					{
						return false;
					}
				}
				else
					return false;
			}
		}
	}

	return true;
}

bool Sat_Solver::Choose(variable*& new_variable)
{
	int most_included = 0, counter = 0;
	for (int j = 0; j < variables_size; j++)
    if (!variables[j].determined)
    if ((variables[j].poslist.size() + variables[j].neglist.size())>counter)
	{
        counter = variables[j].poslist.size() + variables[j].neglist.size();
		most_included = j;
	}
	if (counter == 0 && most_included == 0)
		return false;

	new_variable = &variables[most_included];

	return true;
}

void Sat_Solver::init_var(variable* new_variable,bool task)
{
	if (task){
        int a = new_variable->neglist.size();
        int b = new_variable->poslist.size();
		if (b >= a)
			new_variable->value = true;
		else
			new_variable->value = false;
        new_variable->determined = true;
	}
	else
	{
		if (new_variable->value)
			new_variable->value = false;
		else
			new_variable->value = true;
	}
	
}

void Sat_Solver::Brackets_checking(variable* new_variable)
{
	if (new_variable == NULL)
		return;
	if (new_variable->value)
	{
        for (int i = 0; i < new_variable->poslist.size(); i++)
        if (!new_variable->poslist[i]->checker)
		{
            new_variable->poslist[i]->checker = true;
            local_changes.push_back(new_variable->poslist[i]);
		}
	}
	else
	{
        for (int i = 0; i < new_variable->neglist.size(); i++)
        if (!new_variable->neglist[i]->checker)
		{
            new_variable->neglist[i]->checker = true;
            local_changes.push_back(new_variable->neglist[i]);
		}
	}
}

void Sat_Solver::Back_track(vector<clauses*> local_changes)
{
	for (int i = 0; i < local_changes.size(); i++)
        local_changes[i]->checker = false;
}

void Sat_Solver::Cancellation_of_changes(vector<variable*> local_var_changes)
{
	for (int i = 0; i < local_var_changes.size(); i++)
        local_var_changes[i]->determined = false;
}

bool Sat_Solver::Satsolving(variable* new_variable)
{
    Brackets_checking(new_variable);
    if (Solution_search())
		return true;
    if (!Unit_propagation(new_variable))
	{
        Cancellation_of_changes(local_var_changes);
            Back_track(local_changes);
			local_var_changes.clear();
			local_changes.clear();
		return false;
	}
	var_changes.push_back(local_var_changes);
	local_var_changes.clear();
	changes.push_back(local_changes);
	local_changes.clear();
    if (!Choose(new_variable))
		new_variable = NULL;
	else
	init_var(new_variable,true);
    if (Satsolving(new_variable))
		return true;
	init_var(new_variable,false);
    if (Satsolving(new_variable))
		return true;
	deinit_var(new_variable);
    Cancellation_of_changes(var_changes[var_changes.size() - 1]);
    Back_track(changes[changes.size() - 1]);
	var_changes.pop_back();
	changes.pop_back();
	return false;
}

void Sat_Solver::Pure_initialization()
{
	for (int i = 0; i < variables_size; i++)
	{
        if (variables[i].poslist.size() == 0)
		{
            variables[i].determined = true;
			variables[i].value = false;
            Brackets_checking(&variables[i]);
		}
        if (variables[i].neglist.size() == 0)
		{
            variables[i].determined = true;
			variables[i].value = true;
            Brackets_checking(&variables[i]);
		}
	}
}

bool Sat_Solver::solver()
{
	if (function_size == 0)
		return true;
	if (variables_size == 0)
		return true;
    Pure_initialization();
    if (Satsolving(NULL))
		return true;
	else
		return false;
}
