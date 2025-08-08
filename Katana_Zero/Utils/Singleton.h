#pragma once

template<typename T>
class Singleton
{
	static T* _instance;

public:
	static T* GetInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new T();
		}
		return _instance;
	}
	static void DestroyInstance()
	{
		if (_instance != nullptr)
		{
			delete _instance;
			_instance = nullptr;
		}
	}

protected:
	// �����ڿ� �Ҹ��ڸ� protected�� �����ϸ� �ܺο��� �̱��� ��ü�� ����, �Ҹ��� �� ����.
	Singleton() = default;
	virtual ~Singleton() = default;

	Singleton(const Singleton&) = delete;				// ��������� ����
	Singleton& operator=(const Singleton&) = delete;	// ���Կ����� ����
	// && : rvalue ����
	// �̵� ���� ����
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	virtual void Destroy() {}
};

// static ��� �ʱ�ȭ
template<typename T>
T* Singleton<T>::_instance = nullptr;