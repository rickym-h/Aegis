// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
template <typename InElementType>
struct TPriorityQueueNode {
	InElementType Element;
	float Priority;

	TPriorityQueueNode()
	{
		
	}

	TPriorityQueueNode(InElementType InElement, float InPriority)
	{
		Element = InElement;
		Priority = InPriority;
	}

	bool operator<(const TPriorityQueueNode<InElementType> Other) const
	{
		return Priority < Other.Priority;
	}
};

template <typename InElementType>
class AEGIS_API TPriorityQueue
{
public:
	TPriorityQueue()
	{
		Array.Heapify();
	}
	
	// Always check if IsEmpty() before Pop-ing!
	InElementType Pop()
	{
		TPriorityQueueNode<InElementType> Node;
		Array.HeapPop(Node);
		return Node.Element;
	}

	TPriorityQueueNode<InElementType> PopNode()
	{
		TPriorityQueueNode<InElementType> Node;
		Array.HeapPop(Node);
		return Node;
	}

	void Push(InElementType Element, float Priority)
	{
		Array.HeapPush(TPriorityQueueNode<InElementType>(Element, Priority));
	}

	bool IsEmpty() const
	{
		return Array.Num() == 0;
	}

private:
	TArray<TPriorityQueueNode<InElementType>> Array;
};