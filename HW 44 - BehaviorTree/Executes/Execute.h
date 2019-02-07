#pragma once

class Execute
{
public:
	Execute()
	{

	}
	
	// ��� �߻� Ŭ�������� �Ҹ����� ��� virtual �Ƚᵵ �������� �̷����
	virtual ~Execute() {} // virtual ���ϸ� ȣ�� �ȵ� protected �־

	virtual void Update() = 0;
	
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

	virtual void ResizeScreen() = 0;
};