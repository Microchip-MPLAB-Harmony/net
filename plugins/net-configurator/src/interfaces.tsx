import React, { MouseEvent } from 'react';
export interface IChip {
  cardIndex: string;
  handleCardClick: (E : MouseEvent<HTMLButtonElement> ,T: string) => void;
  handleDragStart: (E: React.DragEvent<HTMLDivElement>, T: string) => void;
  selectedCards: string[];
}

export interface ITemplateHeader1 {
  selectedCards: string[];
  handleMoveToActive: VoidFunction;
}

export interface ITemplateHeader2 {
  selectedCards: string[];
  handleMoveToAvailable: VoidFunction;
}

// TODO : add interface for menu-attributes + API response
export interface IMenuInterface {
  name: string,
  iconName: string,
  isSelected: boolean,
  color: string,
  className: string //  'presentation' | 'transport' |
}


export interface IAPIResponse{

}


export interface IDraggableProps {
  selectedSection: IMenuInterface & IAPIResponse
}