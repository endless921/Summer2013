#include "BlackJack.h"
#include <iostream>
#include <vector>
#include <conio.h>

using namespace std;

BlackJack::BlackJack() : super() { }

void BlackJack::play()
{
	while (true)
	{
		// Create the Dealer, vector of Players, and ask how many Players there will be.
		Dealer * theDealer = new Dealer();
		vector<Player> player_vec;
		int nPlayers;
		cout << "How many players would you like to have? :  "; cin >> nPlayers;
		if (nPlayers > 10)
		{
			cout << "Only 10 Players allowed.\nThe new number of Players is 10." << endl;
			nPlayers = 10;
		}
		else if (nPlayers < 1)
		{
			cout << "You need at least 1 other Player.\nThe new number of Players is 1." << endl;
			nPlayers = 1;
		}
		// Reserve nPlayers amount of Players.
		player_vec.reserve(nPlayers);

		// Create n amount of Players, and make sure the Dealer is last in the vector.
		cout << endl;
		cin.ignore();
		for (int i = 1; i <= nPlayers; ++i)
		{
			string name;
			cout << "Player " << i << "\'s Name :  ";
			getline(cin, name);
			Player pl_i(name);
			player_vec.push_back(pl_i);
		}

		// Give each Player a Card in sequential order or turn.
		for (int i = 0; i < 2; ++i)
		{
			for (unsigned int j = 0; j < player_vec.size(); ++j)
				player_vec[j].takeCard1(theDealer->dealCard());
			
			theDealer->takeCard1(theDealer->dealCard());
		}
	
		// Allow each player to take their respective turn.
		vector<int> ePlayers;
		for (unsigned int i = 0; i < player_vec.size(); ++i)
		{
			cout << endl << endl;
			cout << player_vec[i].getName() << "\'s Turn" << endl;
			player_vec[i].printHand1();
			while (true)
			{
				// Does i-th Player already have 21?
				if (win(countHand(player_vec[i].getHand1())))
				{
					cout << player_vec[i].getName() << "\'s WON!!!" << endl;
					break;
				}
				// Does i-th Player want to Hit or Stay? Hit and continue if they do.
				if (player_vec[i].hit())
				{
					Card tmpCard = theDealer->dealCard();
					player_vec[i].takeCard1(tmpCard);
					cout << tmpCard.toString() << endl << endl; 
				}
				// If i-th Player Stays, then end their turn.
				else
					break;
				// Did i-th Player win?
				if (win(countHand(player_vec[i].getHand1())))
				{
					cout << player_vec[i].getName() << "\'s WON!!!" << endl;
					break;
				}
				// Did i-th Player lose?
				if (bust(countHand(player_vec[i].getHand1())))
				{
					cout << player_vec[i].getName() << " Bust! :(" << endl << endl;
					ePlayers.push_back(i);
					_getch();
					break;
				}
			}
		}
		// Allow the Dealer to take their turn. NULL theDealer if he loses.
		if (!dealersTurn(theDealer))
			theDealer = NULL;
				
		// Get rid of the Player's that already lost, if there are any.
		if (ePlayers.size() > 0)
			erasePlayers(player_vec, ePlayers);
		
		// Print each Player's hand at the end of the game, and announce who won.
		for (unsigned int i = 0; i < player_vec.size(); ++i)
		{
			cout << "***** " << player_vec[i].getName() << "\'s Hand ***** (" << countHand(player_vec[i].getHand1()) << ")" << endl << endl;
			player_vec[i].printHand1();
		}
		// Print the Dealer's hand, if they haven't lost already.
		if (theDealer != NULL)
		{
			cout << "***** The Dealer\'s Hand ***** (" << countHand(theDealer->getHand1()) << ")" << endl;
			theDealer->printHand1();
		}
		// End game, and ask to play again.
		if (!playAgain())
		{
			delete theDealer;
			cout << endl << endl << endl << endl << endl;
			break;
		}
		delete theDealer;
	} 
}
// Count the Player's Hand.
int BlackJack::countHand(vector<Card> hand)
{
	int sum = 0, size = hand.size();
	bool ace11 = false;
	for (int i = 0; i < size; ++i)
	{
		if (hand[i].getRank() == 10 || hand[i].getRank() == 11 || hand[i].getRank() == 12 || hand[i].getRank() == 13)
			sum += 10;
		else if (hand[i].getRank() == 14)
		{
			if (sum > 10)
				sum += 1;
			else
			{
				sum += 11;
				ace11 = true;
			}
		}
		else
			sum += hand[i].getRank();
	}
	if (ace11 && sum > 21)
		sum -= 10;

	return sum;
}

bool BlackJack::bust(int sum)
{
	if (sum > 21)
		return true;

	return false;
}

bool BlackJack::win(int sum)
{
	if (sum == 21)
		return true;

	return false;
}

bool BlackJack::dealersTurn(Dealer *theDealer)
{
	cout << endl << endl;
	cout << "The Dealer\'s Turn" << endl;
	theDealer->printHand1();
	while (true)
	{
		vector<Card> dealersHand = theDealer->getHand1();
		// If the Dealer gets 21.
		if (win(countHand(dealersHand)))
		{
			cout << "The Dealer WON!!!" << endl << endl;
			_getch();
			return true;
		}
		// If the Dealer has less than 17 and less 4 cards, hit.
		if (countHand(dealersHand) < 17)
		{
			Card tmpCard = theDealer->dealCard();
			theDealer->takeCard1(tmpCard);
			cout << "The Dealer hit :  " << tmpCard.toString() << endl << endl;
			_getch();
		}
		// If the Dealer has a "soft" 17, hit.
		else if (countHand(dealersHand) <= 17 && dealersHand.size() >= 4)
		{
			Card tmpCard = theDealer->dealCard();
			theDealer->takeCard1(tmpCard);
			cout << "The Dealer hit :  " << tmpCard.toString() << endl << endl;
			_getch();
		}
		// If the Dealer "stays," end his turn.
		else 
			return true;
		dealersHand = theDealer->getHand1();
		// If the Dealer "hit," check to make sure they won or lost.
		// If the Dealer won.
		if (win(countHand(dealersHand)))
		{
			cout << "The Dealer WON!!!" << endl << endl;
			return true;
		}
		// If the Dealer bust, get rid of them and end their turn.
		if (bust(countHand(dealersHand)))
		{
			cout << "The Dealer BUST! :(" << endl  << endl;
			return false;
		}
	}
}

bool BlackJack::playAgain()
{
	char selection [10];
	while (true)
	{	
		cout << "Would you like to play again? (y/n) :  "; cin >> selection;
		if (selection[0] == 'y' || selection[0] == 'Y')
			return true;
		else if (selection[0] == 'n' || selection[0] == 'N')
			return false;
		else
			cout << "Please choose Y or N." << endl << endl;
	} 
}
void BlackJack::erasePlayers(vector<Player>& players, vector<int> indexes)
{
	unsigned int offset = 0;
	vector<Player>::iterator itr;
	for (unsigned int i = 0; i < indexes.size(); ++i)
	{
		itr = players.begin();
		players.erase(itr + (indexes[i] - offset));
		++offset;
	}
}