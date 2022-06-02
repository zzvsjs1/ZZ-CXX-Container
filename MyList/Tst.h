#ifndef APT_A2_TST
#define APT_A2_TST

#include <cstddef>
#include <cstring>
#include <string>

template <typename CharT, typename Traits = ::std::char_traits<CharT>>
class TstNode final
{
private:

	using Self = TstNode<CharT, Traits>;

public:

	explicit TstNode(const char& letter) noexcept
		: mLeft(),
		mMid(),
		mRight(),
		mParent(),
		mLetter(letter),
		mEndWord(false)
	{ }

	TstNode(const char& letter, Self* const parent) noexcept
		: mLeft(),
		mMid(),
		mRight(),
		mParent(parent),
		mLetter(letter),
		mEndWord(false)
	{ }

	~TstNode() noexcept = default;

	Self* mLeft;

	Self* mMid;

	Self* mRight;

	Self* mParent;

	CharT mLetter;

	bool mEndWord;

};


template <typename CharT, typename Traits = ::std::char_traits<CharT>>
class TernarySearchTree final
{
private:

	using Node = TstNode<CharT, Traits>;

public:
	
	using node_type = Node;
	using value_type = CharT;
	using reference = CharT&;
	using const_reference = const CharT&;
	using pointer = CharT*;
	using const_pointer = const CharT*;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using string_type = ::std::basic_string<CharT, Traits>;
	using traits = Traits;


	TernarySearchTree() noexcept
		: mRoot(), mSize()
	{ }

	template <typename InputIterator>
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

	TernarySearchTree(TernarySearchTree&& other) noexcept
		: mRoot(other.mRoot), mSize(other.mSize)
	{
		other.mRoot = nullptr;
		other.mSize = 0;
	}

	~TernarySearchTree() noexcept
	{
		deleteSubTree(mRoot);
	}

private:

	static bool charEqual(const CharT& left, const CharT& right)
	{
		return traits::eq(left, right);
	}

	static bool charNonEqual(const CharT& left, const CharT& right)
	{
		return !charEqual(left, right);
	}

	static bool charLessThan(const CharT& left, const CharT& right)
	{
		return traits::lt(left, right);
	}

	static bool charLessThanEqualTo(const CharT& left, const CharT& right)
	{
		return !charLessThan(right, left);
	}

	static bool charGreaterThan(const CharT& left, const CharT& right)
	{
		/* 
		 * NOLINT(readability-suspicious-call-argument).
		 *
		 * We need to reverse the argument in here.
		 */
		return charLessThan(right, left);
	}

	static bool charGreaterEqualTo(const CharT& left, const CharT& right)
	{
		return !charLessThan(left, right);
	}

public:

	template <typename InputIterator>
	void buildDictionary(InputIterator first, InputIterator last)
	{
		for (; first != last; ++first)
		{
			addWord(*first);
		}
	}

	bool contain(const string_type& string) const noexcept
	{
		return contain(string.c_str(), string.length());
	}

	bool contain(
		const value_type* string, 
		const size_type length
	) const noexcept
	{
		Node* cur = mRoot;
		bool end = false;
		bool ret = false;
		size_type i = 0;

		while (cur && !end)
		{
			if (charLessThan(string[i], cur->mLetter))
			{
				cur = cur->mLeft;
			}
			else if (charGreaterThan(string[i], cur->mLetter))
			{
				cur = cur->mRight;
			}
			else
			{
				if (i == length - 1)
				{
					end = true;
				}
				else
				{
					cur = cur->mMid;
					++i;
				}
			}
		}

		if (cur)
		{
			ret = cur->mEndWord;
		}

		return ret;
	}

	bool addWord(const string_type& word)
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
						if (charLessThan(word[i], cur->mLetter))
						{
							if (!cur->mLeft)
							{
								cur->mLeft = new Node(word[i], cur);
							}

							cur = cur->mLeft;
						}
						else if (
							charGreaterThan(word[i], cur->mLetter)
							)
						{
							if (!cur->mRight)
							{
								cur->mRight = new TstNode(word[i], cur);
							}

							cur = cur->mRight;
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
								if (!cur->mMid)
								{
									// Throw.
									cur->mMid = new TstNode(word[i], cur);
								}

								cur = cur->mMid;
							}
						}
					}
				}
				catch (...)
				{
					// Failed to create a node, we will do clean up.
					clearUpUpwards(cur);
					throw;
				}

				// If end word, return false. Adding failed.
				if (cur->mEndWord)
				{
					ret = false;
				}
				else
				{
					cur->mEndWord = true;
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

	bool deleteWord(const string_type& word) noexcept
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
				if (charLessThan(word[i], cur->mLetter))
				{
					cur = cur->mLeft;
				}
				else if (charGreaterThan(word[i], cur->mLetter))
				{
					cur = cur->mRight;
				}
				else
				{
					if (i == length - 1)
					{
						done = true;
					}
					else
					{
						cur = cur->mMid;
						++i;
					}
				}
			}

			// No search all letter, cur is nullptr or cur is not end word. return false.
			if (i != length - 1 || !cur || !cur->mEndWord)
			{
				ret = false;
			}
			else
			{
				// Do delete.
				cur->mEndWord = false;
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
		Node* ret = node->mRight;
		while (ret->mLeft)
		{
			ret = ret->mLeft;
		}

		return ret;
	}

	void clearUpUpwards(Node* cur) noexcept
	{
		bool done = false;

		while (cur && !done)
		{
			if (cur->mMid || cur->mEndWord)
			{
				done = true;
			}
			else
			{
				if (!cur->mLeft)
				{
					transplantBToA(cur, cur->mRight);
				}
				else if (!cur->mRight)
				{
					transplantBToA(cur, cur->mLeft);
				}
				else
				{
					Node* successor = getSuccessorsFromRight(cur);

					if (successor->mParent != cur)
					{
						transplantBToA(successor, successor->mRight);
						successor->mRight = cur->mRight;
						successor->mRight->mParent = successor;
					}

					transplantBToA(cur, successor);
					successor->mLeft = cur->mLeft;
					successor->mLeft->mParent = successor;
				}

				const Node* const toDelete = cur;
				cur = cur->mParent;

				delete toDelete;
			}
		}
	}

	void transplantBToA(Node* const a, Node* const b) noexcept
	{
		if (!a->mParent)
		{
			mRoot = b;
		}
		else if (a == a->mParent->mLeft)
		{
			a->mParent->mLeft = b;
		}
		else if (a == a->mParent->mRight)
		{
			a->mParent->mRight = b;
		}
		else
		{
			a->mParent->mMid = b;
		}

		if (b)
		{
			b->mParent = a->mParent;
		}
	}

	void emptyInsert(const CharT* word, const size_type length)
	{
		// Throw
		mRoot = new Node(word[0]);
		Node* cur = mRoot;

		try
		{
			for (size_type i = 1; i < length; ++i)
			{
				cur->mMid = new Node(word[i], cur);
				cur = cur->mMid;
			}
		}
		catch (...) 
		{
			/* 
			 * If we failed to create a tree when
			 * the tree is empty before.
			 *
			 * We will destroy all the nodes which
			 * already created.
			 */
			deleteSubTree(mRoot);
			throw;
		}

		cur->mEndWord = true;
	}

	static void deleteSubTree(Node* root) noexcept
	{
		if (root)
		{
			if (root->mLeft)
			{
				deleteSubTree(root->mLeft);
				root->mLeft = nullptr;
				
			}

			if (root->mMid)
			{
				deleteSubTree(root->mMid);
				root->mMid = nullptr;
			}

			if (root->mRight)
			{
				deleteSubTree(root->mRight);
				root->mRight = nullptr;
			}

			delete root;
		}
	}

	Node* mRoot;

	size_type mSize;

};

using StringTst = TernarySearchTree<char>;

#endif // !APT_A2_TST