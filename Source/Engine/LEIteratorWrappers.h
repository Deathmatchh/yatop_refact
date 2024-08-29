#ifndef LEIteratorWrappers_H
#define LEIteratorWrappers_H

template <class T>
class LEMapIterator
{
private:
	typename T::iterator mCurrent;
	typename T::iterator mEnd;
	/// ˽�й��캯������Ϊ�������ṩ����
	MapIterator() {};
public:
	typedef typename T::mapped_type MappedType;
	typedef typename T::key_type KeyType;

	/** ���캯��.
	@remarks
		�ṩstart��end���������ڳ�ʼ��.
	*/
	MapIterator(typename T::iterator start, typename T::iterator end)
		: mCurrent(start), mEnd(end)
	{
	}

	bool hasMoreElements(void) const
	{
		return mCurrent != mEnd;
	}

	typename T::mapped_type getNext(void)
	{
		return (mCurrent++)->second;
	}
	typename T::mapped_type peekNextValue(void)
	{
		return mCurrent->second;
	}
	typename T::key_type peekNextKey(void)
	{
		return mCurrent->first;
	}
	MapIterator<T> & operator=( MapIterator<T> &rhs )
	{
		mCurrent = rhs.mCurrent;
		mEnd = rhs.mEnd;
		return *this;
	}
	typename T::pointer peekNextValuePtr(void)
	{
		return &(mCurrent->second);
	}
	void moveNext(void)
	{
		mCurrent++;
	}



};

#endif