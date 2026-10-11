#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <memory>
#include <conio.h>


enum CardSuit
{   // Name // glyph from the emoji font // Color
    Club, // 3 // Black
    Diamond, // 4 // Red
    Heart, // 2 // Red
    Spade // 1 // Black
};

std::string ToString(const CardSuit& cs)
{
    switch (cs)
    {
    case CardSuit::Club:
        return "Clubs";
        break;
    case CardSuit::Diamond:
        return "Diamonds";
        break;
    case CardSuit::Heart:
        return "Hearts";
        break;
    case CardSuit::Spade:
        return "Spades";
        break;
    }

    return "Null";
}

struct Card
{
    Card(const std::string& name, const uint16_t& value): Name(name), Value(value)
    {}

    const bool IsNull() const { return Value == 0 || Name == "" || cardID == -1; }

    const std::string GetCardSuit() const 
    {
        switch (Suit)
        {
        case Club:
            return "3";
            break;
        case Diamond:
            return "4";
            break;
        case Heart:
            return "2";
            break;
        case Spade:
            return "1";
            break;
        }

        return "NULL";
    }

    std::string Name{ "" };
    CardSuit Suit{ CardSuit::Spade };
    uint16_t Value{ 0 };
    std::string Abbreviature{ "" };
    sf::Color SuitColor{ sf::Color::Red };
    int cardID{ -1 };
};

struct Deck
{
    Deck() { InitDeck();}

    const std::vector<Card>& GetDeck() { return deck; }

    void ShuffleDeck() 
    {
        Randomize();
    }

    void ShowDeck() 
    {
        for (const Card& card : deck)
        {
            std::cout << "Name: " << card.Name << " Suit: " << ToString(card.Suit) << " Value: " << card.Value << "\n";
        }
    }

    void DealCards(std::vector<Card>& playerDeck, std::vector<Card>& npcDeck)
    {
        while (deck.size() != 0)
        {
            std::random_device device;
            std::mt19937 rng(device());
            std::uniform_int_distribution<std::mt19937::result_type> distribute(0, deck.size() - 1);

            size_t index = distribute(rng);

            Card dealtCard = deck.at(index);

            if (deck.size() % 2 == 0)
            {
                playerDeck.push_back(dealtCard);
            }
            
            if (deck.size() % 2 == 1)
            {
                npcDeck.push_back(dealtCard);
            }

            auto iter = std::find_if(deck.begin(), deck.end(), [dealtCard](const Card& card) { return card.cardID == dealtCard.cardID; });

            deck.erase(iter);
        }
    }

private:
    std::vector<Card> deck;

    void InitDeck()
    {
        int currentSuitIndex = 0;
        CardSuit CurrentSuit = CardSuit::Spade;
        sf::Color CurrentSuitColor = sf::Color::Black;

        for (int i = 1; i < 53; ++i)
        {
            Card currentCard("", 0);

            
            switch (i % 13)
            {
            case 0:
                currentCard.Value = 13;
                currentCard.Name = "King";
                currentCard.Abbreviature = "K";
                break;

            case 1:
                currentSuitIndex++;
                ChangeSuit(currentSuitIndex, CurrentSuit);
                currentCard.Name = "Ace";
                currentCard.Value = 14;
                currentCard.Abbreviature = "A";
                break;
            
            case 11 :
                currentCard.Name = "Jack";
                currentCard.Value = 11;
                currentCard.Abbreviature = "J";
                break;

            case 12:
                currentCard.Name = "Queen";
                currentCard.Value = 12;
                currentCard.Abbreviature = "Q";
                break;
            
            default:
                currentCard.Value = i % 13;
                currentCard.Abbreviature = std::to_string(currentCard.Value);
                currentCard.Name = std::to_string(currentCard.Value);          
                break;
            }
            currentCard.Suit = CurrentSuit;

            // check if the current suit is a black suit
            CurrentSuit == CardSuit::Club || CurrentSuit == CardSuit::Spade ? CurrentSuitColor = sf::Color::Black : CurrentSuitColor = sf::Color::Red;

            currentCard.SuitColor = CurrentSuitColor;

            currentCard.cardID = i;

            deck.push_back(currentCard);
        }

        ShuffleDeck();
    }
    void ChangeSuit(int index, CardSuit& OutSuit)
    {
        switch (index)
        {
        case 1:
            OutSuit = CardSuit::Spade;
            break;

        case 2:
            OutSuit = CardSuit::Heart;
            break;

        case 3:
            OutSuit = CardSuit::Club;
            break;

        case 4:
            OutSuit = CardSuit::Diamond;
            break;
        }
    }
    void Randomize()
    {
        for (size_t i = deck.size() - 1; i > 0; i--)
        {
            std::random_device device;
            std::mt19937 rng(device());
            std::uniform_int_distribution<std::mt19937::result_type> distribute(0, i);

            size_t j = distribute(rng);

            SwapCards(deck.at(i), deck.at(j));
        }
    }
    void SwapCards(Card& a, Card& b) 
    {
        Card temp = a;
        a = b;
        b = temp;
    }
};

struct Player
{
    Player(const std::string& id) : ID(id) {};

    std::vector<Card>& GetDeck() { return deck; }
    const std::vector<Card>& GetHand() { return hand; }
    const std::vector<Card>& GetTroops() { return troops; }
    const std::vector<Card>& GetDiscardPile() { return discardPile; }
    const Card& GetActiveCard() { return activeCard; }
    const bool GetIsPlayerAlive() const { return isAlive; }
    const std::string& GetID() const { return ID; }

    void SetActiveCard(const Card& newCard)
    {
        activeCard = newCard;
    }
    void DisplayHand()
    {
        for (const Card& card : hand)
        {
            std::cout << "Name: " << card.Name << " Suit: " << ToString(card.Suit) << " Owner: " << GetID() << "\n";
        }
    }
    void DisplayTroops() 
    {
        if (troops.empty())
        {
            return;
        }
        
        std::cout << "--------- " << ID << " Troops --------- \n";
        
        for (const Card& card : troops)
        {
            std::cout << card.Name << " " << ToString(card.Suit) << "\n";
        }
    }

    void RefillHand() 
    {
        while (hand.size() < 3)
        {
            DrawCard();

            if (deckAndDPEmpty)
            {
                break;
            }
        }
    }
    void PlayCardFromHand(int index)
    {
        if (index > hand.size())
        {
            index--;
        }

        if (index <= 0 && hand.empty() && deckAndDPEmpty)
        {
            isAlive = false;
            std::cout << ID << " has lost the game! \n";
            return;
        }

        if (activeCard.IsNull())
        {
            activeCard = hand.at(index);
        }
        else
        { 
            // make a copy, so there are no issues when the card at the index is being deleted
            Card troop = hand.at(index);
            troops.push_back(troop);
        }

        auto iter = std::find_if(hand.begin(), hand.end(), [&](const Card& card) {return card.cardID == hand[index].cardID; });
        hand.erase(iter);
    }
    void AddToDeck(const Card& card)
    {
        deck.push_back(card);
    }
    void DrawCard()
    {
        if (!isAlive)
        {
            return;
        }

        if (deck.empty())
        {
            if (discardPile.empty())
            {
                deckAndDPEmpty = true;
                return;
            }

            for (Card card : discardPile)
            {
                deck.push_back(card);
            }

            discardPile = {};
            deckAndDPEmpty = false;
        }

        std::random_device device;
        std::mt19937 rng(device());
        std::uniform_int_distribution<std::mt19937::result_type> distribute(0, deck.size() - 1);

        size_t index = distribute(rng);

        Card DrawnCard = deck.at(index);

        hand.push_back(DrawnCard);

        auto iter = std::find_if(deck.begin(), deck.end(), [DrawnCard](const Card& card) { return card.cardID == DrawnCard.cardID; });

        deck.erase(iter);
    }
    void AddToDiscardPile(const std::vector<Card>& table)
    {
        for (size_t i = 0; i < table.size(); i++)
        {
            discardPile.push_back(table[i]);
        }
    }
    void PrepForWar()
    {
        RefillHand();

        size_t handsize = hand.size();

        for (size_t i = 0; i < handsize; i++)
        {
            Card troop = hand[0];

            troops.push_back(troop);

            hand.erase(hand.begin());
        }

        if (troops.empty())
        {
            isAlive = false;
        }
    }

    void ResetTroops()
    {
        troops = {};
    }


protected:
    bool isAlive{ true };
    bool deckAndDPEmpty{ false };
    Card activeCard = { "", 0 };
    std::string ID;
    std::vector<Card> hand;
    std::vector<Card> troops;
    std::vector<Card> deck;
    std::vector<Card> discardPile;
};

struct AI : Player 
{
    AI(const std::string& id) : Player(id){}
    void PlayCard(uint16_t& npcIndex)
    {
        std::random_device device;
        std::mt19937 rng(device());
        std::uniform_int_distribution<std::mt19937::result_type> distribute(0, hand.size() - 1);
        uint16_t index = distribute(rng);

        npcIndex = index;

        PlayCardFromHand(index);
    }
};

struct CardWidget : sf::Drawable
{
    CardWidget(const sf::Vector2f& widgetPos, const Card& c, sf::Font& font, sf::Font& suitFont, sf::RenderWindow& w) : card(c), window(w)
    {
        float newSizeX = window.getSize().x / defaultCardSize.x;
        float newSizeY = window.getSize().y / defaultCardSize.y;

        sf::Vector2f newSize;
        newSize.x = defaultCardSize.x - newSizeX;
        newSize.y = defaultCardSize.y - newSizeY;

        CardShape.setFillColor(BackgroundColor);
        CardShape.setSize(newSize);
        CardShape.setPosition(widgetPos);

        EmptyShape.setOutlineColor(BackgroundColor);
        EmptyShape.setOutlineThickness(5);
        EmptyShape.setFillColor(sf::Color::Transparent);
        EmptyShape.setSize(newSize);
        EmptyShape.setPosition(widgetPos);

        CardAbbreviation.setFont(font);
        CardAbbreviation.setString(c.Abbreviature);
        CardAbbreviation.setFillColor(c.SuitColor);
        CardAbbreviation.setCharacterSize(15);
        CardAbbreviation.setPosition(CardShape.getPosition() + sf::Vector2f(10.f, 10.f));

        TopCardSuit.setFont(suitFont);
        TopCardSuit.setString(c.GetCardSuit());
        TopCardSuit.setFillColor(c.SuitColor);
        TopCardSuit.setCharacterSize(15);
        TopCardSuit.setPosition(CardShape.getPosition() + sf::Vector2f(25.f, 10.f));

        BottomCardAbbreviation.setFont(font);
        BottomCardAbbreviation.setString(c.Abbreviature);
        BottomCardAbbreviation.setFillColor(c.SuitColor);
        BottomCardAbbreviation.setCharacterSize(15);
        BottomCardAbbreviation.setPosition(CardShape.getPosition() + (CardShape.getSize() - sf::Vector2f(10.f, 10.f)));
        BottomCardAbbreviation.setRotation(180);

        BottomCardSuit.setFont(suitFont);
        BottomCardSuit.setString(c.GetCardSuit());
        BottomCardSuit.setFillColor(c.SuitColor);
        BottomCardSuit.setCharacterSize(15);
        BottomCardSuit.setPosition(CardShape.getPosition() + (CardShape.getSize() - sf::Vector2f(25.f, 10.f)));
        BottomCardSuit.setRotation(180);

        CardBack.setFillColor(SleeveColor);
        CardBack.setSize(newSize);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        if (isFacedDown)
        {
            target.draw(CardBack);
            return;
        }

        if (isEmpty)
        {
            target.draw(EmptyShape);
        }
        else
        {
            target.draw(CardShape);
            target.draw(CardAbbreviation);
            target.draw(TopCardSuit);
            target.draw(BottomCardAbbreviation);
            target.draw(BottomCardSuit); 
        }
        
    }

    void UpdateCardPosition(const sf::Vector2f& newPos)
    {
        CardShape.setPosition(newPos);
        CardBack.setPosition(newPos);
        EmptyShape.setPosition(newPos);

        BottomCardAbbreviation.setPosition(CardShape.getPosition() + (CardShape.getSize() - sf::Vector2f(10.f, 10.f)));
        BottomCardSuit.setPosition(CardShape.getPosition() + (CardShape.getSize() - sf::Vector2f(25.f, 10.f)));
        CardAbbreviation.setPosition(CardShape.getPosition() + sf::Vector2f(10.f, 10.f));
        TopCardSuit.setPosition(CardShape.getPosition() + sf::Vector2f(25.f, 10.f));
    }

    const sf::RectangleShape& GetCardShape() const { return CardShape; }
    Card& GetCard() { return card; }

    void SetIsFacedDown(const bool& IsFacedDown)
    {
        isFacedDown = IsFacedDown;
    }
    void SetIsEmpty(const bool& IsEmtpy)
    {
        isEmpty = IsEmtpy;
    }
    void SetCard(std::shared_ptr<CardWidget> InCard)
    {
        isEmpty = false;
        card = InCard->GetCard();
        InCard->UpdateCardPosition(CardShape.getPosition());
        CardAbbreviation = InCard->CardAbbreviation;
        BottomCardAbbreviation = InCard->BottomCardAbbreviation;
        TopCardSuit = InCard->TopCardSuit;
        BottomCardSuit = InCard->BottomCardSuit;
    }


public:
    sf::Text CardAbbreviation;
    sf::Text BottomCardAbbreviation;
    sf::Text TopCardSuit; 
    sf::Text BottomCardSuit;

private:
    bool isFacedDown{ false };
    bool isEmpty{ false };
    sf::RectangleShape CardShape;
    sf::RectangleShape EmptyShape;
    sf::RectangleShape CardBack;
    sf::Vector2f defaultCardSize{ 100, 150};
    sf::Color BackgroundColor{ sf::Color::White };
    sf::Color SleeveColor{ sf::Color::Cyan };
    sf::RenderWindow& window;
    Card card;
};

struct PlayerWidget : sf::Drawable
{
    PlayerWidget(Player& p, sf::Font& f, sf::Font& sf, sf::RenderWindow& w) : player(p), font(f), suitFont(sf), window(w)
    {
        player = p;

        Card card("", 0);
        discardPile = std::make_unique<CardWidget>(sf::Vector2f(0.f, 0.f), card, font, suitFont, window);
        discardPile->SetIsFacedDown(true);
        
        activeCard = std::make_unique<CardWidget>(sf::Vector2f(0.f, 0.f), card, font, suitFont, window);
        activeCard->SetIsEmpty(true);
        
        DeckSize = std::make_unique<sf::Text>("", font);
        DiscardPileSize = std::make_unique<sf::Text>("", font);

        PlayerWon = std::make_unique < sf::Text>("", font);
        PlayerWon->setFillColor(sf::Color::Yellow);

        if(p.GetID() == "player")
        { 
            discardPile->UpdateCardPosition(sf::Vector2f(window.getSize().x - (discardPile->GetCardShape().getSize().x + 300), window.getSize().y - discardPile->GetCardShape().getSize().y));
            activeCard->UpdateCardPosition(sf::Vector2f((window.getSize().x / 2) - (activeCard->GetCardShape().getSize().x / 2), (window.getSize().y / 2) + (activeCard->GetCardShape().getSize().y / 2)));
            DeckSize->setPosition(sf::Vector2f(0.f, window.getSize().y - 50.f));
        }
        else
        {
            discardPile->UpdateCardPosition(sf::Vector2f(300, 0));
            activeCard->UpdateCardPosition(sf::Vector2f((window.getSize().x / 2) - (activeCard->GetCardShape().getSize().x / 2), (window.getSize().y / 2) - (activeCard->GetCardShape().getSize().y)));
            DeckSize->setPosition(sf::Vector2f());
        }
        DiscardPileSize->setPosition(discardPile->GetCardShape().getPosition() + sf::Vector2f(50.f, 0.f));
        PlayerWon->setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
    }

    void BeginPlay()
    {
        GetPlayerHand();
        UpdatePileSizeText();
    }

    void PlayCardFromHand(const uint16_t& index)
    {
        if (!player.GetIsPlayerAlive())
        {
            std::cout << "[PlayCardFromHand] " << player.GetID() << " has lost the game \n";
            return;
        }

        if (playerHand.at(index)->GetCard().IsNull())
        {
            return;
        }

        CardWidget& card = *playerHand[index];
        Card& c = card.GetCard();

        card.SetIsEmpty(true);
        activeCard->SetCard(playerHand[index]);
    }

    void GetPlayerHand()
    {
        if (!player.GetIsPlayerAlive())
        {
            std::cout << "[PlayCardFromHand] " << player.GetID() << " has lost the game \n";
            return;
        }

        playerHand = {};

        for (size_t i = 0; i < player.GetHand().size(); i++)
        {
            const Card& card = player.GetHand()[i];

            if (card.IsNull())
            {
                continue;
            }

            std::shared_ptr<CardWidget> cardWidget = std::make_shared<CardWidget>(sf::Vector2f(0.f, 0.f), card, font, suitFont, window);

            playerHand.push_back(cardWidget);
        }

        UpdateHandPosition();
    }

    void UpdateHandPosition()
    {
        sf::Vector2f cardPos{};
        
        if (playerHand.empty())
        {
            return;
        }

        cardPos.x = window.getSize().x / 3.f;

        if (player.GetID() == "player")
        {
            cardPos.y = window.getSize().y - (playerHand[0]->GetCardShape().getSize().y);
        }

        for (std::shared_ptr<CardWidget> card : playerHand)
        {
            card->UpdateCardPosition(cardPos);

            cardPos.x += card->GetCardShape().getSize().x + 15;
        }
    }

    void GetPlayerTroops()
    {
        if (!player.GetIsPlayerAlive())
        {
            std::cout << "[PlayCardFromHand] " << player.GetID() << " has lost the game \n";

            std::string Winner{ "" };
            if (player.GetID() == "player")
            {
                Winner = "npc";
            }
            else
            {
                Winner = "player";
            }

            std::string WinnerMessage = Winner + " won the game!";
            PlayerWon->setString(WinnerMessage);
            return;
        }

        playerTroops = {};

        for (size_t i = 0; i < player.GetTroops().size(); i++)
        {
            const Card& card = player.GetTroops()[i];

            std::shared_ptr<CardWidget> cardWidget = std::make_shared<CardWidget>(sf::Vector2f(0.f, 0.f), card, font, suitFont, window);

            playerTroops.push_back(cardWidget);
        }

        UpdateTroopsPosition();
    }

    void UpdateTroopsPosition()
    {
        sf::Vector2f cardPos{};

        cardPos.x = (window.getSize().x / 2.f) + 200.f;
        cardPos.y = playerTroops[0]->GetCardShape().getSize().y;

        if (player.GetID() == "player")
        {
            cardPos.y = window.getSize().y - (playerTroops[0]->GetCardShape().getSize().y * 2);
        }

        for (std::shared_ptr<CardWidget> card : playerTroops)
        {
            card->UpdateCardPosition(cardPos);

            cardPos.x += card->GetCardShape().getSize().x + 15;
        }
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        for (const std::shared_ptr<CardWidget> card : playerHand)
        {
            target.draw(*card);
        }
        
        for (const std::shared_ptr<CardWidget> card : playerTroops)
        {
            target.draw(*card);
        }

        target.draw(*activeCard);
        target.draw(*discardPile);
        target.draw(*DeckSize);
        target.draw(*DiscardPileSize);
        target.draw(*PlayerWon);
    }

    void Resolve()
    {
        activeCard->SetIsEmpty(true);
        playerTroops = {};

        UpdatePileSizeText();
    }

    void UpdatePileSizeText()
    {
        DiscardPileSize->setString(std::to_string(player.GetDiscardPile().size()));
        DeckSize->setString(std::to_string(player.GetDeck().size()));
    }

    std::vector<std::shared_ptr<CardWidget>> playerHand;
    std::vector<std::shared_ptr<CardWidget>> playerTroops;
    std::unique_ptr<CardWidget> discardPile;
    std::unique_ptr<CardWidget> activeCard;

    std::unique_ptr<sf::Text> DeckSize;
    std::unique_ptr<sf::Text> DiscardPileSize;
    std::unique_ptr<sf::Text> PlayerWon;

    Player& player;
    sf::Font& font;
    sf::Font& suitFont;

    sf::RenderWindow& window;
};

struct WidgetManager : sf::Drawable
{
    WidgetManager(std::shared_ptr<Player> p, std::shared_ptr<Player> ai, sf::Font& f, sf::Font& sf, sf::RenderWindow& window) : font(f)
    {
        playerWidget = std::make_unique<PlayerWidget>(*p, f, sf, window);
        npcWidget = std::make_unique<PlayerWidget>(*ai, f, sf, window);

        Table.setSize(sf::Vector2f(window.getSize().x - 300.f, window.getSize().y - 300.f));
        Table.setFillColor(sf::Color::Green);
        Table.setOrigin(sf::Vector2f(Table.getSize().x / 2, Table.getSize().y / 2));
        Table.setPosition(sf::Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f));
    }
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(Table);
        target.draw(*playerWidget);
        target.draw(*npcWidget); 
    }

    void BeginPlay()
    {
        playerWidget->BeginPlay();
        npcWidget->BeginPlay();
    }

    void PlayCard(const uint16_t& index, const uint16_t& npcIndex)
    {
        playerWidget->PlayCardFromHand(index);
        npcWidget->PlayCardFromHand(npcIndex);
    }

    void ResolveWar()
    {
        playerWidget->Resolve();
        npcWidget->Resolve();
    }

    void UpdateHand()
    {
        playerWidget->GetPlayerHand();
        npcWidget->GetPlayerHand();
    }

    void UpdateTroops()
    {
        playerWidget->GetPlayerTroops();
        npcWidget->GetPlayerTroops();
    }

    std::unique_ptr<PlayerWidget> playerWidget;
    std::unique_ptr<PlayerWidget> npcWidget;

    sf::RectangleShape Table;

    sf::Font& font;
};


struct WarResolver
{
    WarResolver(std::shared_ptr<Player> p, std::shared_ptr<Player> Npc, bool& WarResolved) : player(p), npc(Npc), bWarResolved(WarResolved) {};

    void War(WidgetManager& widgetManager)
    {
        std::cout << "Player played: " << player->GetActiveCard().Value << " of " << ToString(player->GetActiveCard().Suit) << " font glyph " << player->GetActiveCard().GetCardSuit() << " VS npc played : " << npc->GetActiveCard().Value << " of " << ToString(npc->GetActiveCard().Suit) << " font glyph " << npc->GetActiveCard().GetCardSuit() << "\n";

        Card playerActiveCard = player->GetActiveCard();
        Card npcActiveCard = npc->GetActiveCard();

        std::vector<Card> table{ playerActiveCard, npcActiveCard };

        if (!bWarResolved)
        {
            std::cout << "- ------------!UNRESOLVED WAR!------------ - \n";

            for (size_t i = 0; i < player->GetTroops().size(); i++)
            {
                Card playerTroop = player->GetTroops()[i];
                Card npcTroop = npc->GetTroops()[i];

                table.push_back(playerTroop);
                table.push_back(npcTroop);
            }

            widgetManager.UpdateTroops();
            player->DisplayTroops();
            npc->DisplayTroops();

            if (player->GetTroops().size() < 3)
            {
                bPlayerWon = false;
                bWarResolved = true;
                return;
            }
            else if (npc->GetTroops().size() < 3)
            {
                bPlayerWon = true;
                bWarResolved = true;
                return;
            }

            if (player->GetTroops()[2].Value > npc->GetTroops()[2].Value)
            {
                player->AddToDiscardPile(table);
                bPlayerWon = true;
                bWarResolved = true;
            }

            if (player->GetTroops()[2].Value < npc->GetTroops()[2].Value)
            {
                npc->AddToDiscardPile(table);
                bPlayerWon = false;
                bWarResolved = true;
            }

            if (player->GetTroops()[2].Value == npc->GetTroops()[2].Value)
            {
                uint16_t playerTroopSum = 0, npcTroopSum = 0;

                for (size_t i = 0; i < player->GetTroops().size(); i++)
                {
                    playerTroopSum += player->GetTroops()[i].Value;
                    npcTroopSum += npc->GetTroops()[i].Value;
                }

                if (playerTroopSum > npcTroopSum)
                {
                    player->AddToDiscardPile(table);
                    bPlayerWon = true;
                }

                if (playerTroopSum < npcTroopSum)
                {
                    npc->AddToDiscardPile(table);
                    bPlayerWon = false;
                }

                if (playerTroopSum == npcTroopSum)
                {
                    if (player->GetTroops()[2].Suit > npc->GetTroops()[2].Suit)
                    {
                        player->AddToDiscardPile(table);
                        bPlayerWon = true;
                    }
                    else
                    {
                        npc->AddToDiscardPile(table);
                        bPlayerWon = false;
                    }
                }
            }

            Card emptyCard("", 0);
            player->SetActiveCard(emptyCard);
            npc->SetActiveCard(emptyCard);

            player->RefillHand();
            npc->RefillHand();

            player->ResetTroops();
            npc->ResetTroops();
            bWarResolved = true;

            return;
        }

        if (player->GetActiveCard().Value > npc->GetActiveCard().Value)
        {
            player->AddToDiscardPile(table);
            bPlayerWon = true;
            bWarResolved = true;
        }

        if (npc->GetActiveCard().Value > player->GetActiveCard().Value)
        {
            npc->AddToDiscardPile(table);
            bPlayerWon = false;
            bWarResolved = true;
        }

        if (npc->GetActiveCard().Value == player->GetActiveCard().Value)
        {
            std::cout << "Draw Or War! \n";
            bWarResolved = false;

            player->PrepForWar();
            npc->PrepForWar();

            War(widgetManager);
            return;
        }

        Card emptyCard("", 0);
        player->SetActiveCard(emptyCard);
        npc->SetActiveCard(emptyCard);
    }

private:
    bool bPlayerWon{ false };
    bool& bWarResolved;
    std::shared_ptr<Player> player{ nullptr };
    std::shared_ptr<Player> npc{ nullptr };
};

struct GameManager 
{
    GameManager(std::shared_ptr<Player> p, std::shared_ptr<AI> ai, WidgetManager& wm) : player(p), npc(ai), widgetManager(wm)
    {
        WR = std::make_unique<WarResolver>(p, ai, bWarResolved);
    }

    void UpdateHand()
    {
        player->RefillHand();
        npc->RefillHand(); 

        widgetManager.UpdateHand();
    }

    void PlayCard(const uint16_t& cardIndex, uint16_t& npcIndex) 
    {
        player->PlayCardFromHand(cardIndex);
        npc->PlayCard(npcIndex);

        widgetManager.PlayCard(cardIndex, npcIndex);
    }

    void CommenceWar(WidgetManager& wm) 
    {
        WR->War(wm);
    }

    const bool& GetIsWarResolved() const { return bWarResolved; }

private:
    bool bWarResolved;
    std::unique_ptr<WarResolver> WR;
    std::shared_ptr<Player> player;
    std::shared_ptr<AI> npc;
    WidgetManager& widgetManager;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(1440, 720), "WarxD");

    Deck deck;

    bool warResolved = true;

    sf::Font font;
    sf::Font emojiFont;

    if(!font.loadFromFile("Assets/Fonts/arvo/Arvo-Regular.ttf")) { std::cout << "Font not loaded\n"; }
    if(!emojiFont.loadFromFile("Assets/Fonts/icons/SuitIcons-Regular.otf")) { std::cout << "Font not loaded\n"; }


    std::shared_ptr<Player> player = std::make_shared<Player>("player");
    std::shared_ptr<AI> npc = std::make_shared<AI>("npc");


    deck.DealCards(player->GetDeck(), npc->GetDeck());

    WidgetManager WM{ WidgetManager(player, npc, font, emojiFont, window) };
    GameManager GM{ GameManager(player, npc, WM) };

    for (size_t i = 0; i < 3; i++)
    {
        player->DrawCard();
        npc->DrawCard();
    }

    player->DisplayHand();
    npc->DisplayHand();

    WM.BeginPlay();

    bool bIsGameOn = true, bIsWarUnresolved = false;

    uint16_t index = 0, npcIndex = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyReleased && bIsGameOn)
            {
                if (event.key.code == sf::Keyboard::Num1 && !bIsWarUnresolved)
                {
                    index = 1;
                }
                else if (event.key.code == sf::Keyboard::Num2 && !bIsWarUnresolved)
                {
                    index = 2;
                }
                else if (event.key.code == sf::Keyboard::Num3 && !bIsWarUnresolved)
                {
                    index = 3;
                }
                else if (event.key.code == sf::Keyboard::Space && bIsWarUnresolved)
                {
                    WM.ResolveWar();
                    system("cls");
                    player->DisplayHand();
                    npc->DisplayHand();
                    bIsWarUnresolved = false;

                    GM.UpdateHand();
                    //WM.UpdateHand();

                    continue;
                }
                else 
                {
                    continue;
                }
                
                if (!bIsWarUnresolved)
                {
                    GM.PlayCard(index - 1, npcIndex);
                    //WM.PlayCard(index - 1, npcIndex);

                    GM.CommenceWar(WM);
                    bIsWarUnresolved = true;

                    bIsGameOn = GM.GetIsWarResolved() && (player->GetIsPlayerAlive() && npc->GetIsPlayerAlive());

                    // reset it so it does not remember the last index
                    index = 0;
                }
            }
        }

        window.clear();
        window.draw(WM);
        window.display();
    }
}