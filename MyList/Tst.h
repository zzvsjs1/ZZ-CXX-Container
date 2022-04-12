#ifndef APT_A2_TST
#define APT_A2_TST

#include <type_traits>
#include <cstddef>
#include <cstring>
#include <string>
#include "Healper.h"


class Node
{
private:

	using Self = Node;
	using CharT = char;

public:

	explicit Node(const char& letter) noexcept
		: mLeft(),
		mMid(),
		mRight(),
		mParent(),
		mLetter(letter),
		mEndWord(false)
	{ }

	Node(const char& letter, Self* const parent) noexcept
		: mLeft(),
		mMid(),
		mRight(),
		mParent(parent),
		mLetter(letter),
		mEndWord(false)
	{ }

	Self*& left() noexcept
	{
		return mLeft;
	}

	Self* const& left() const noexcept
	{
		return mLeft;
	}

	Self*& right() noexcept
	{
		return mRight;
	}

	Self* const& right() const noexcept
	{
		return mRight;
	}

	Self*& mid() noexcept
	{
		return mMid;
	}

	Self* const& mid() const noexcept
	{
		return mMid;
	}

	Self*& parent() noexcept
	{
		return mParent;
	}

	Self* const& parent() const noexcept
	{
		return mParent;
	}

	CharT& letter() noexcept
	{
		return mLetter;
	}

	const CharT& letter() const noexcept
	{
		return mLetter;
	}

	bool& endWord() noexcept
	{
		return mEndWord;
	}

	const bool& endWord() const noexcept
	{
		return mEndWord;
	}

private:

	Self* mLeft;

	Self* mMid;

	Self* mRight;

	Self* mParent;

	CharT mLetter;

	bool mEndWord;

};


class TernarySearchTree
{
private:

	using CharT = char;

public:

	using node_type = Node;
	using value_type = CharT;
	using reference = CharT&;
	using const_reference = const CharT&;
	using pointer = CharT*;
	using const_pointer = const CharT*;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	TernarySearchTree() noexcept
		: mRoot(), mSize()
	{ }

	template <typename InputIterator, typename = ::jstd::RequireInputIter<InputIterator>>
	TernarySearchTree(InputIterator first, InputIterator last)
		: mRoot(), mSize()
	{
		try
		{
			buildDictionary(first, last);
		}
		catch (...)
		{
			deleteSubTree(mRoot);
			throw;
		}
	}

	~TernarySearchTree() noexcept
	{
		deleteSubTree(mRoot);
	}

	template <typename InputIterator, typename = ::jstd::RequireInputIter<InputIterator>>
	void buildDictionary(InputIterator first, InputIterator last)
	{
		for (; first != last; ++first)
		{
			addWord(*first);
		}
	}

	bool search(const ::std::string& string) const noexcept
	{
		return search(string.c_str(), string.length());
	}

	bool search(const CharT* string) const noexcept
	{
		return search(string, ::std::strlen(string));
	}

	bool search(const CharT* string, const size_type length) const noexcept
	{
		Node* cur = mRoot;
		bool end = false;
		bool ret = false;
		size_type i = 0;

		while (cur && !end)
		{
			if (string[i] < cur->letter())
			{
				cur = cur->left();
			}
			else if (string[i] > cur->letter())
			{
				cur = cur->right();
			}
			else
			{
				if (i == length - 1)
				{
					end = true;
				}
				else
				{
					cur = cur->mid();
					++i;
				}
			}
		}

		if (cur)
		{
			ret = cur->endWord();
		}

		return ret;
	}

	bool addWord(const ::std::string& word)
	{
		return addWord(word.c_str(), word.length());
	}

	bool addWord(const CharT* word)
	{
		return addWord(word, ::std::strlen(word));
	}

	bool addWord(const CharT* word, const size_type length)
	{
		bool ret = false;
		if (length != 0)
		{
			if (empty())
			{
				emptyInsert(word, length);
				++mSize;
				ret = true;
			}
			else
			{
				Node* cur = mRoot;

				try
				{
					size_type i = 0;
					bool done = false;
					while (i < length && !done)
					{
						if (word[i] < cur->letter())
						{
							if (!cur->left())
							{
								cur->left() = new Node(word[i], cur);
							}

							cur = cur->left();
						}
						else if (word[i] > cur->letter())
						{
							if (!cur->right())
							{
								cur->right() = new Node(word[i], cur);
							}

							cur = cur->right();
						}
						else
						{
							++i;

							if (i == length)
							{
								done = true;
							}
							else
							{
								if (!cur->mid())
								{
									cur->mid() = new Node(word[i], cur);
								}

								cur = cur->mid();
							}
						}
					}
				}
				catch (...)
				{
					clearUpUpwards(cur);
					throw;
				}

				// If end word, return false. Adding failed.
				if (cur->endWord())
				{
					ret = false;
				}
				else
				{
					cur->endWord() = true;
					++mSize;
					ret = true;
				}
			}
		}

		return ret;
	}

	bool empty() const noexcept
	{
		return mSize == 0;
	}

	size_type size() const noexcept
	{
		return mSize;
	}

	bool deleteWord(const ::std::string& word) noexcept
	{
		return deleteWord(word.c_str(), word.length());
	}

	bool deleteWord(const CharT* word) noexcept
	{
		return deleteWord(word, ::std::strlen(word));
	}

	bool deleteWord(const CharT* word, const size_type length) noexcept
	{
		bool ret = false;

		if (word && length != 0 && !empty())
		{
			size_type i = 0;
			Node* cur = mRoot;
			bool done = false;

			while (cur && !done)
			{
				if (word[i] < cur->letter())
				{
					cur = cur->left();
				}
				else if (word[i] > cur->letter())
				{
					cur = cur->right();
				}
				else
				{
					if (i == length - 1)
					{
						done = true;
					}
					else
					{
						cur = cur->mid();
						++i;
					}
				}
			}

			// No search all letter, cur is nullptr or cur is not end word. return false.
			if (i != length - 1 || !cur || !cur->endWord())
			{
				ret = false;
			}
			else
			{
				// Do delete.
				cur->endWord() = false;
				clearUpUpwards(cur);
				--mSize;
				ret = true;
			}
		}

		return ret;
	}

private:

	static Node* getSuccessorsFromRight(Node* node) noexcept
	{
		Node* ret = node->right();
		while (ret->left())
		{
			ret = ret->left();
		}

		return ret;
	}

	void clearUpUpwards(Node* cur) noexcept
	{
		bool done = false;

		while (cur && !done)
		{
			if (cur->mid() || cur->endWord())
			{
				done = true;
			}
			else
			{
				if (!cur->left())
				{
					transplantBToA(cur, cur->right());
				}
				else if (!cur->right())
				{
					transplantBToA(cur, cur->left());
				}
				else
				{
					Node* successor = getSuccessorsFromRight(cur);
					if (successor->parent() != cur)
					{
						transplantBToA(successor, successor->right());
						successor->right() = cur->right();
						successor->right()->parent() = successor;
					}

					transplantBToA(cur, successor);
					successor->left() = cur->left();
					successor->left()->parent() = successor;
				}

				const Node* const toDelete = cur;
				cur = cur->parent();
				delete toDelete;
			}
		}
	}

	void transplantBToA(Node* const a, Node* const b) noexcept
	{
		if (!a->parent())
		{
			mRoot = b;
		}
		else if (a == a->parent()->left())
		{
			a->parent()->left() = b;
		}
		else if (a == a->parent()->right())
		{
			a->parent()->right() = b;
		}
		else
		{
			a->parent()->mid() = b;
		}

		if (b)
		{
			b->parent() = a->parent();
		}
	}

	void emptyInsert(const CharT* word, const size_type length)
	{
		// Throw
		mRoot = new Node(word[0]);
		Node* cur = mRoot;


		for (size_type i = 1; i < length; ++i)
		{
			cur->mid() = new Node(word[i], cur);
			cur = cur->mid();
		}

		cur->endWord() = true;
	}

	static void deleteSubTree(Node* root) noexcept
	{
		if (root)
		{
			if (root->left())
			{
				deleteSubTree(root->left());
				root->left() = nullptr;
				
			}

			if (root->mid())
			{
				deleteSubTree(root->mid());
				root->mid() = nullptr;
			}

			if (root->right())
			{
				deleteSubTree(root->right());
				root->right() = nullptr;
			}

			delete root;
		}
	}

	Node* mRoot;

	size_type mSize;

};



#endif // !APT_A2_TST