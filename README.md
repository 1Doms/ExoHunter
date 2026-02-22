# ExoHunter



GAMEPLAY ---



H Host Server

C Client connect

P Stop Server



ARCHITECTURE ---



Server -> Net6ServerSubsystem -> OpcodeRouter -> CommandHandler(Validate) -> GameMode



Client -> Net6ClientSubsystem -> OpcodeRouter -> CommandHandler -> GameState(Replicate)





other :



Enet6Lib -> Network lib



ExoHunterType.h -> Variable Enum, Opcode



PacketBuilder -> PacketStruct, PacketBuilder



NetworkSerialization -> Serialization/Deserialization binaire



Net6BaseSubsystem -> Parent GameInstance (Server, Client sont des childs)



OpcodeRouter -> Sépare Packet en deux (Opcode, Payload) -> éxécute HandleStruct\[Opcode](Payload){}



CommandHandler -> Contient les HandleStruct, Sert à valider avant de passer dans le GameMode/State



GameMode -> La replication du Server



GameState -> Replication coté client





POSTMORTEM ---



Lundi : énorme bug de version entre 5.7 et vs, j'avais aussi des bugs quand je passais sous 5.4, j'arrivais pas à savoir quel était la bonne version de vs qu'il fallait avoir, j'ai perdu énormément de temps dessus, point positive j'ai setup Enet6 en 1 heure



Mardi : (Problème perso mais j'ai pu régler ca) je suis passer sur mon PC Fixe (Seigneur comment ça allait vite) j'ai continué à programmer malgré des problèmes de build (que j'ai ignoré)



Mercredi : j'ai fini les fonctions de serialization



Jeudi : de retour sur mon petit PC le matin, j'étais coincé avec le bug de SetEnv qui disait que le chemin d'accès était trop long, j'avais beau chercher depuis le début de projet (C'est à dire lundi) à essayer de corriger ça, je ne comprenais pas d'où ca venait j'ai fait plein de manipulation pour rallonger la lecture des PATHs. Du coup jeudi j'ai passé la journée à traquer ce bug car le projet voulait plus du tout se lancer, au final j'ai vu que la solution serait apparemment de passer sous Rider ce que j'ai fait, j'ai pris tellement de temps à l'init encore des bugs partout, Riderlinker qui voulait pas se synchro (j'ai fais sans), au final le bug de SetEnv est un bait, car même avec ca d'afficher le problème est autre part, en fait le projet ne voulait juste pas se lancer car il y avait des problèmes de compil des ; mal déclarés ou oublie de forward. Mais je sais pas pourquoi ça ne voulait pas les affichés et me disait que c'était SetEnv..



Vendredi, Samedi, Dimanche : Tout se passe bien j'ai pu continuer tranquillement j'avoue avoir pris beaucoup de temps sur l'opmisation, j'ai travaillé matin et soir, je suis assez fier même si j'ai pas réussi à faire un jeu :)) Merci pour tes cours !

