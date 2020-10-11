#define _CRT_SECURE_NO_WARNINGS
#include <bits/stdc++.h>
#include <unordered_set>

#define EPSILON "epsilon"
#define ENDOFLINE "$"

class NoTerminal
{
public:
	std::string name;
	std::vector<std::vector<std::string>> branches;
	std::vector<int> posBranchs;
	bool first;
	NoTerminal() {
		first = false;
	};
	NoTerminal(std::string n) {
		name = n;
	};
	void addBranch(std::string name, std::vector<std::string>& branch, int pos) {
		branches.push_back(branch);
		posBranchs.push_back(pos);
	}
};

class NoterminalElements
{
public:
	std::map<std::string, NoTerminal> map_noterminal;
	NoterminalElements() {}
	NoterminalElements(std::map<std::string, NoTerminal>& elementsNoTerminal) {
		map_noterminal = elementsNoTerminal;
	}

	bool isNoTerminal(std::string target) {
		return map_noterminal.find(target) != map_noterminal.end();
	}

	NoTerminal find(std::string key) {
		return map_noterminal[key];
	}

};


class FirstElement
{
public:
	bool epsilon;
	std::string name;
	std::unordered_set<std::string> elements;

	FirstElement() {
		epsilon = false;
	}

	FirstElement(std::string n) {
		name = n;
	}

};

class First
{
private:
	std::map<std::string, FirstElement> map_first;
	NoterminalElements noterminal_elements;

	void expandEpsilon() {
		for (auto& pairFirstElement : noterminal_elements.map_noterminal) {
			std::string name = pairFirstElement.first;
			FirstElement& firstEl = map_first[name];
			firstEl.name = name;
			std::map<std::string, bool> ignore;
			ignore[name] = true;
			if (findEpsilon(pairFirstElement.second, ignore)) {
				firstEl.epsilon = true;
			}
		}
	}

	bool findEpsilon(NoTerminal& target, std::map<std::string, bool>& ignore) {
		FirstElement& elementFirst = map_first[target.name];
		if (elementFirst.epsilon)
			return true;
		bool epsilon = false;
		for (auto const& branch : target.branches) {
			//search in a branch
			bool epsilonBranch = true;
			std::map<std::string, bool> ignoreBranch = ignore;
			for (auto const& token : branch) {
				if (noterminal_elements.isNoTerminal(token))//is Noterminal
				{
					if (ignoreBranch[token])
					{
						epsilonBranch = false;
						break;
					}
					else {
						ignoreBranch[token] = true;
						if (!findEpsilon(noterminal_elements.map_noterminal[token], ignoreBranch))
						{
							epsilonBranch = false;
							break;
						}
					}
				}
				else//is a terminal element
				{
					if (token != EPSILON)
					{
						epsilonBranch = false;
						break;
					}
				}
			}
			epsilon |= epsilonBranch;
		}
		//shearh in all the branches and dont find a epsilon
		return epsilon;
	}

	void findFirstNoTerminal(NoTerminal& noterminal, std::unordered_set<std::string>& solution, std::unordered_set<std::string>& ignore) {
		std::string name = noterminal.name;
		if (ignore.find(name) != ignore.end())
			return;
		auto& branches = noterminal.branches;
		ignore.insert(name);
		for (auto& branch : branches) {
			for (auto& token : branch) {
				if (noterminal_elements.isNoTerminal(token))
				{
					FirstElement& first_token = map_first[token];
					NoTerminal& noterminal_token = noterminal_elements.map_noterminal[token];
					if (first_token.epsilon)
					{
						findFirstNoTerminal(noterminal_token, solution, ignore);
					}
					else
					{
						findFirstNoTerminal(noterminal_token, solution, ignore);
						break;
					}
				}
				else//terminal
				{
					if (token != EPSILON)
						solution.insert(token);
					break;
				}
			}
		}
	}

	void findFirst() {
		expandEpsilon();
		for (auto& pairFirstElement : noterminal_elements.map_noterminal) {
			std::string name = pairFirstElement.first;
			NoTerminal& noterminal = pairFirstElement.second;
			std::unordered_set<std::string>& first = map_first[name].elements;
			std::unordered_set<std::string> ignore;
			findFirstNoTerminal(noterminal, first, ignore);
		}
	}
public:
	First() {}

	First(NoterminalElements& noterminal) {
		noterminal_elements = noterminal;
		findFirst();
	}


	FirstElement find(std::string key) {
		return map_first[key];
	}

};



class NextElement
{
public:
	std::string name;
	std::unordered_set<std::string> elements;

	NextElement() {
	}

	NextElement(std::string n) {
		name = n;
	}

};

class Next {
public:
	std::map<std::string, NextElement> map_next;
	Next(NoterminalElements& noterminal, First& fir)
	{
		noterminal_elements = noterminal;
		first = fir;
		findNext();
	}
	NextElement find(std::string key) {
		return map_next[key];
	}

private:
	void findNext()
	{
		for (auto& pairFirstElement : noterminal_elements.map_noterminal) {
			std::string name = pairFirstElement.first;
			NoTerminal& noterminal = pairFirstElement.second;
			NextElement& nextEl = map_next[name];
			nextEl.name = name;
			std::unordered_set<std::string>& next = nextEl.elements;

			std::unordered_set<std::string> ignore;
			findNextNoTerminal(noterminal, next, ignore);
		}
	}
	NoterminalElements noterminal_elements;
	First first;

	std::string nextToken(int i, int branch_size, std::vector<std::string>& branch) {
		std::string next_token;
		if (i + 1 < branch_size) next_token = branch[i + 1];
		else next_token = EPSILON;
		return next_token;
	}


	bool findNexBeta(std::unordered_set<std::string>& solution, int i, int branch_size, std::vector<std::string>& branch) {
		bool hasEpsilon = true;
		for (i; i < branch_size; i++) {
			std::string next_token = nextToken(i, branch_size, branch);
			if (noterminal_elements.isNoTerminal(next_token))
			{
				FirstElement first_noterminal_element = first.find(next_token);
				solution.insert(first_noterminal_element.elements.begin(), first_noterminal_element.elements.end());
				if (!first_noterminal_element.epsilon) {
					hasEpsilon = false;
					break;
				}
			}
			else
			{
				if (next_token != EPSILON) {
					solution.insert(next_token);
					hasEpsilon = false;
					break;
				}
			}
		}
		return hasEpsilon;
	}

	void findNextNoTerminal(NoTerminal& noterminal, std::unordered_set<std::string>& solution, std::unordered_set<std::string>& ignore)
	{
		std::string& name = noterminal.name;
		if (ignore.find(name) != ignore.end())
			return;
		ignore.insert(name);

		if (noterminal.first)
			solution.insert(ENDOFLINE);

		for (auto& pairFirstElement : noterminal_elements.map_noterminal) {
			//TODOMAIN: BOTTLENECK
			NoTerminal& noterminal_search = pairFirstElement.second;
			auto& branchs = noterminal_search.branches;
			for (auto& branch : branchs)
			{
				int branch_size = branch.size();
				for (int i = 0; i < branch_size; i++)
				{
					std::string& token = branch[i];
					//TODOMAIN: BOTTLENECK
					if (token == name)
					{
						bool hasEpsilon = findNexBeta(solution, i, branch_size, branch);
						if (hasEpsilon)
						{
							findNextNoTerminal(noterminal_search, solution, ignore);
						}
					}
				}
			}
		}
	}

};


class Prediction {
private:
	std::vector<std::unordered_set<std::string>> prediction;
	void findSets(NoterminalElements& noterminal_elements, First& first, Next& next, int number_of_rules) {
		prediction.resize(number_of_rules);
		std::map<std::string, NoTerminal>& noterminal_list = noterminal_elements.map_noterminal;
		for (auto& pairElement : noterminal_list) {
			std::string name = pairElement.first;
			NoTerminal& noterminal = pairElement.second;
			auto& branches = noterminal.branches;
			int sizeBranches = branches.size();
			for (int num_branch = 0; num_branch < sizeBranches; num_branch++) {
				auto& branch = branches[num_branch];
				bool epsilon = true;
				auto& predictionElement = prediction[noterminal.posBranchs[num_branch]];
				for (auto& token : branch) {
					if (noterminal_elements.isNoTerminal(token))
					{
						FirstElement first_noterminal_element = first.find(token);
						predictionElement.insert(first_noterminal_element.elements.begin(), first_noterminal_element.elements.end());
						if (!first_noterminal_element.epsilon)
						{
							epsilon = false;
							break;
						}
					}
					else
					{
						if (token != EPSILON)
						{
							predictionElement.insert(token);
							epsilon = false;
							break;
						}
					}
				}
				if (epsilon)
				{
					NextElement nextNoTerminal = next.find(name);
					predictionElement.insert(nextNoTerminal.elements.begin(), nextNoTerminal.elements.end());
				}
			}
		}
	}

public:
	Prediction(NoterminalElements& noterminal_elements, int number_of_rules) {
		First first(noterminal_elements);
		Next next(noterminal_elements, first);
		findSets(noterminal_elements, first, next, number_of_rules);
	}

	std::vector<std::unordered_set<std::string>> getPredictionSets() {
		return prediction;
	}

};



void addElementNoterminal(std::string& s, std::map<std::string, NoTerminal>& map_noterminal, std::map<std::string, NoTerminal>::iterator& it, int counter) {
	int n = s.size();
	std::vector<std::string> lstring;
	std::string name = "";
	for (int i = 0; i < n; i++)
	{
		if (s[i] == ' ')
			continue;
		std::string temp = "";
		for (i; i < n; i++)
		{
			if (s[i] == ' ')
				break;
			temp.push_back(s[i]);
		}
		if (name == "")
			name = temp;
		else
			lstring.push_back(temp);
	}
	it = map_noterminal.find(name);
	if (it == map_noterminal.end())
		map_noterminal[name].name = name;
	map_noterminal[name].addBranch(name, lstring, counter);
}

std::pair<int, NoterminalElements> read() {
	std::map<std::string, NoTerminal> map_noterminal;
	std::string s;
	std::map<std::string, NoTerminal>::iterator it;
	bool first = true;
	std::getline(std::cin, s);
	addElementNoterminal(s, map_noterminal, it, 0);
	map_noterminal.begin()->second.first = true;
	int counter = 1;
	while (std::getline(std::cin, s)) {
		addElementNoterminal(s, map_noterminal, it, counter);
		counter++;
	}
	return std::pair<int, NoterminalElements>(counter, NoterminalElements(map_noterminal));
}

int main()
{
	std::freopen("in.txt", "r", stdin);
	std::freopen("out.txt", "w", stdout);
	std::pair<int, NoterminalElements> int_noter = read();
	int number_of_rules = int_noter.first;
	NoterminalElements noterminal_elements = int_noter.second;
	Prediction prediction(noterminal_elements, number_of_rules);

	for (auto& set : prediction.getPredictionSets()) {
		for (auto& token : set)
			std::cout << token << " ";
		std::cout << '\n';
	}

	return 0;
}

