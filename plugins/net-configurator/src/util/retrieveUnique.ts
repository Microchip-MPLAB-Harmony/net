export const retrieveUnique = (cards: string[]): string[] => {
  const tempArr = Array.from(new Set(cards));
  return tempArr;
};

export const setUnsetDraggingColor = (
  divRef: string,
  setType: "set" | "unset"
) => {
  const node = document.getElementById(divRef);
  if (!node) {
    return;
  }
  if (!node?.firstChild) {
    return;
  }
  switch (setType) {
    case "set":
      (node.firstChild as HTMLElement).style.backgroundColor = "lightgreen";
      break;
    case "unset":
      (node.firstChild as HTMLElement).style.backgroundColor = "white";
      break;
  }
};
